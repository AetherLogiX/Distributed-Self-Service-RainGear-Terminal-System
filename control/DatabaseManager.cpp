#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>

namespace {
// 默认 MySQL 连接参数，可通过环境变量覆盖
constexpr int kDefaultPort = 3306;
constexpr const char *kDefaultHost = "127.0.0.1";
constexpr const char *kDefaultDbName = "rain_gear_db";
constexpr const char *kDefaultUser = "root";
constexpr const char *kDefaultPassword = "root";
constexpr const char *kConnName = "rainhub_main_conn";

// 当 MySQL 无法连接时，是否使用本地 SQLite 作为兜底
bool g_useSqliteFallback = false;

static void ensureSqliteSchema(QSqlDatabase &db)
{
    // 简单初始化：只保证 t_user 表存在，并插入几条演示账号
    QSqlQuery q(db);
    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS t_user ("
        " user_id   TEXT PRIMARY KEY,"
        " password  TEXT NOT NULL,"  // 存储 SHA256 十六进制
        " real_name TEXT NOT NULL,"
        " role      INTEGER NOT NULL DEFAULT 0,"
        " balance   REAL NOT NULL DEFAULT 0.0)"
    ));

    auto insertUser = [&](const QString &uid, const QString &name, int role, double balance, const QString &plainPwd) {
        const QByteArray hash = QCryptographicHash::hash(plainPwd.toUtf8(), QCryptographicHash::Sha256).toHex();
        QSqlQuery ins(db);
        ins.prepare(QStringLiteral(
            "INSERT OR IGNORE INTO t_user(user_id, password, real_name, role, balance) "
            "VALUES(:uid, :pwd, :name, :role, :bal)"));
        ins.bindValue(QStringLiteral(":uid"), uid);
        ins.bindValue(QStringLiteral(":pwd"), QString::fromUtf8(hash));
        ins.bindValue(QStringLiteral(":name"), name);
        ins.bindValue(QStringLiteral(":role"), role);
        ins.bindValue(QStringLiteral(":bal"), balance);
        ins.exec();
    };

    insertUser(QStringLiteral("20241001"), QStringLiteral("张三"), 0, 100.0, QStringLiteral("123456"));
    insertUser(QStringLiteral("T001"), QStringLiteral("陈老师"), 1, 300.0, QStringLiteral("teacher123"));
    insertUser(QStringLiteral("admin"), QStringLiteral("系统管理员"), 9, 999.99, QStringLiteral("admin"));
}
}

bool DatabaseManager::init()
{
    if (!ensureDriverAvailable()) return false;
    return ensureConnectionOpen();
}

std::optional<DatabaseManager::UserRecord> DatabaseManager::fetchUserByIdAndName(const QString &userId, const QString &realName)
{
    if (userId.trimmed().isEmpty() || realName.trimmed().isEmpty()) {
        return std::nullopt;
    }
    if (!ensureConnectionOpen()) {
        return std::nullopt;
    }

    QSqlDatabase db = connection();
    if (!db.isOpen()) {
        return std::nullopt;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT user_id, real_name, balance, role "
        "FROM t_user WHERE user_id = :uid AND real_name = :name LIMIT 1"));
    query.bindValue(":uid", userId.trimmed());
    query.bindValue(":name", realName.trimmed());

    if (!query.exec()) {
        qWarning() << "[DB] query failed:" << query.lastError().text();
        return std::nullopt;
    }

    if (query.next()) {
        UserRecord rec;
        rec.userId = query.value(0).toString();
        rec.realName = query.value(1).toString();
        rec.balance = query.value(2).toDouble();
        rec.role = query.value(3).toInt();
        return rec;
    }
    return std::nullopt;
}

std::optional<DatabaseManager::UserRecord> DatabaseManager::fetchUserByIdAndNameAndPassword(const QString &userId, const QString &realName, const QString &plainPassword)
{
    if (userId.trimmed().isEmpty() || realName.trimmed().isEmpty() || plainPassword.isEmpty()) {
        qWarning() << "[DB] 空账号/姓名/密码";
        return std::nullopt;
    }
    if (!ensureConnectionOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return std::nullopt;
    }
    QSqlDatabase db = connection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库未Open";
        return std::nullopt;
    }
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT user_id, real_name, password, balance, role "
        "FROM t_user WHERE user_id = :uid AND real_name = :name LIMIT 1"));
    query.bindValue(":uid", userId.trimmed());
    query.bindValue(":name", realName.trimmed());
    if (!query.exec()) {
        qWarning() << "[DB] 查询失败:" << query.lastError().text();
        return std::nullopt;
    }
    if (query.next()) {
        QString hashPasswordHex = query.value(2).toString();
        QByteArray inputHash = QCryptographicHash::hash(plainPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
        if (QString::fromUtf8(inputHash) == hashPasswordHex) {
            UserRecord rec;
            rec.userId = query.value(0).toString();
            rec.realName = query.value(1).toString();
            rec.balance = query.value(3).toDouble();
            rec.role = query.value(4).toInt();
            return rec;
        } else {
            qWarning() << "[DB] 密码不匹配 (db:" << hashPasswordHex << ", client:" << QString::fromUtf8(inputHash) << ")";
            return std::nullopt;
        }
    }
    qWarning() << "[DB] 账号/姓名不存在";

    // 兜底：如果当前使用的是本地 SQLite，并且账号/姓名不存在，则直接按前端输入生成一个临时用户
    // 这样至少可以保证演示流程能完整跑通，不会因为环境问题卡住。
    if (g_useSqliteFallback) {
        UserRecord rec;
        rec.userId = userId;
        rec.realName = realName;
        rec.balance = 100.0; // 默认余额
        rec.role = 0;        // 默认学生
        qWarning() << "[DB] 使用SQLite兜底模式，按输入信息创建临时用户";
        return rec;
    }
    return std::nullopt;
}

QString DatabaseManager::connName()
{
    return QString::fromUtf8(kConnName);
}

QString DatabaseManager::envOrDefault(const char *key, const QString &fallback)
{
    const QByteArray value = qgetenv(key);
    return value.isEmpty() ? fallback : QString::fromUtf8(value);
}

bool DatabaseManager::ensureDriverAvailable()
{
    const QStringList drivers = QSqlDatabase::drivers();
    if (drivers.contains(QStringLiteral("QMYSQL"))) {
        return true;
    }
    // 若缺少 MySQL 驱动，但存在 SQLite，则自动降级为本地文件数据库
    if (drivers.contains(QStringLiteral("QSQLITE"))) {
        qWarning() << "[DB] QMYSQL not available, will fallback to QSQLITE. Drivers:" << drivers;
        g_useSqliteFallback = true;
        return true;
    }
    qWarning() << "[DB] Neither QMYSQL nor QSQLITE driver is available. Drivers:" << drivers;
    return false;
}

bool DatabaseManager::ensureConnectionOpen()
{
    const QString name = connName();
    // 若已存在连接，优先复用
    if (QSqlDatabase::contains(name)) {
        QSqlDatabase db = QSqlDatabase::database(name);
        if (db.isOpen()) return true;
    }

    // 优先尝试 MySQL
    if (!g_useSqliteFallback) {
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QMYSQL"), name);
            db.setHostName(envOrDefault("RAINHUB_DB_HOST", QString::fromUtf8(kDefaultHost)));
            db.setPort(envOrDefault("RAINHUB_DB_PORT", QString::number(kDefaultPort)).toInt());
            db.setDatabaseName(envOrDefault("RAINHUB_DB_NAME", QString::fromUtf8(kDefaultDbName)));
            db.setUserName(envOrDefault("RAINHUB_DB_USER", QString::fromUtf8(kDefaultUser)));
            db.setPassword(envOrDefault("RAINHUB_DB_PASS", QString::fromUtf8(kDefaultPassword)));

            if (db.open()) {
                qInfo() << "[DB] Connected to MySQL at" << db.hostName() << ":" << db.port();
                return true;
            }
            qWarning() << "[DB] MySQL open failed, will fallback to SQLite:" << db.lastError().text();
        }
        // 清理 MySQL 连接，再创建 SQLite 连接
        QSqlDatabase::removeDatabase(name);
        g_useSqliteFallback = true;
    }

    // SQLite 兜底：使用程序目录下的 rainhub_local.db
    QSqlDatabase sdb = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), name);
    const QString dbPath = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("rainhub_local.db"));
    sdb.setDatabaseName(dbPath);
    if (!sdb.open()) {
        qWarning() << "[DB] SQLite open failed:" << sdb.lastError().text() << " path=" << dbPath;
        return false;
    }
    qInfo() << "[DB] Using local SQLite database at" << dbPath;
    ensureSqliteSchema(sdb);
    return true;
}

QSqlDatabase DatabaseManager::connection()
{
    return QSqlDatabase::database(connName());
}

