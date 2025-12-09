#pragma once

#include <QString>

// 控制层通用返回结果（封装为类，提供更清晰的接口）
class OperationResult {
public:
    enum class Code : int {
        Ok         = 200,
        BadRequest = 400,
        NotFound   = 404,
        Conflict   = 409,
        ServerErr  = 500,
    };

    static OperationResult ok(const QString &msg = QStringLiteral("成功")) {
        return OperationResult(Code::Ok, msg);
    }
    static OperationResult fail(Code c, const QString &msg) {
        return OperationResult(c, msg);
    }
    static OperationResult fail(int c, const QString &msg) { // 兼容旧调用
        return OperationResult(static_cast<Code>(c), msg);
    }

    bool is_ok() const { return code_ == Code::Ok; }
    int code() const { return static_cast<int>(code_); }
    const QString &message() const { return message_; }

private:
    OperationResult(Code c, const QString &msg) : code_(c), message_(msg) {}

    Code code_;
    QString message_;
};
