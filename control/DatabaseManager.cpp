#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QCoreApplication>

namespace {
// 默认连接参数，可通过环境变量覆盖
constexpr int kDefaultPort = 3306;
constexpr const char *kDefaultHost = "127.0.0.1";
constexpr const char *kDefaultDbName = "rain_gear_db";
constexpr const char *kDefaultUser = "root";
constexpr const char *kDefaultPassword = "123456";
constexpr const char *kConnName = "rainhub_mysql_conn";
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
    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QMYSQL"))) {
        qWarning() << "[DB] QMYSQL driver not available. Current drivers:" << QSqlDatabase::drivers();
        return false;
    }
    return true;
}

bool DatabaseManager::ensureConnectionOpen()
{
    const QString name = connName();
    QSqlDatabase db;
    if (QSqlDatabase::contains(name)) {
        db = QSqlDatabase::database(name);
    } else {
        db = QSqlDatabase::addDatabase(QStringLiteral("QMYSQL"), name);
        db.setHostName(envOrDefault("RAINHUB_DB_HOST", QString::fromUtf8(kDefaultHost)));
        db.setPort(envOrDefault("RAINHUB_DB_PORT", QString::number(kDefaultPort)).toInt());
        db.setDatabaseName(envOrDefault("RAINHUB_DB_NAME", QString::fromUtf8(kDefaultDbName)));
        db.setUserName(envOrDefault("RAINHUB_DB_USER", QString::fromUtf8(kDefaultUser)));
        db.setPassword(envOrDefault("RAINHUB_DB_PASS", QString::fromUtf8(kDefaultPassword)));
    }

    if (db.isOpen()) return true;
    if (!db.open()) {
        qWarning() << "[DB] open failed:" << db.lastError().text();
        return false;
    }
    return true;
}

QSqlDatabase DatabaseManager::connection()
{
    return QSqlDatabase::database(connName());
}

