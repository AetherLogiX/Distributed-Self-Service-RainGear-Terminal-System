/*
用户类,用于余额校验等客户端/服务端通用逻辑。
这里的用户种类有学生、教职工和管理员三种，分别对应role 0,1,9
*/
#pragma once

#include <QString>
#include <memory>

class RainGear; // 前置声明，避免头文件循环依赖

class User {
public:
    User(QString uid, QString name, double balance, int role):id(std::move(uid)),name(std::move(name)),balance(balance),role(role){}

    const QString& get_id() const { return id; }
    const QString& get_name() const { return name; }
    double get_balance() const { return balance; }
    int get_role() const { return role; }
    
    //检查能不能借雨具
    bool canBorrow() const { return balance >= 20.0; }

    //在一卡通内进行押金扣款
    bool deduct(double amount) { 
        if (balance>=amount) {
            balance-=amount;
            return true;
        }
        return false;
    }

    //押金返回
    void recharge(double amount) { balance+=amount; }

    // 借伞后由控制层托管给用户（独占所有权）
    void take_gear(std::unique_ptr<RainGear> gear) { current_gear = std::move(gear); }
    bool has_gear() const { return current_gear != nullptr; }
    const RainGear* borrowed_gear() const { return current_gear.get(); }
    std::unique_ptr<RainGear> return_gear() { return std::move(current_gear); }

private:
    QString id;
    QString name;
    double balance;
    int role; //0:学生,1:教职工,9:管理员
    std::unique_ptr<RainGear> current_gear; // 当前借用的雨具（可扩展为列表）
};

