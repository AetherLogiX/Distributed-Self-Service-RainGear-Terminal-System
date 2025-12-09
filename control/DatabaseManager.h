#pragma once

#include <optional>
#include <QString>

class User;
class QSqlDatabase;

// 轻量级数据库助手：负责管理到本地 MySQL8 的连接，并提供基础查询。
// 目前仅在客户端单线程场景下使用，不涉及连接池。
class DatabaseManager {
public:
    struct UserRecord {
        QString userId;
        QString realName;
        double balance {0.0};
        int role {0}; // 0:学生,1:教职工,9:管理员
    };

    // 初始化数据库连接（若已存在则复用）。返回是否成功。
    static bool init();

    // 查询指定学号/工号与姓名的用户，找到则返回用户记录。
    static std::optional<UserRecord> fetchUserByIdAndName(const QString &userId, const QString &realName);

private:
    static QString connName();
    static QString envOrDefault(const char *key, const QString &fallback);
    static bool ensureDriverAvailable();
    static bool ensureConnectionOpen();
    static QSqlDatabase connection();
};

