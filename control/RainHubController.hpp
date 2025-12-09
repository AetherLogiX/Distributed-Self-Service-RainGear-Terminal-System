#pragma once

#include <QString>
#include <memory>

#include "model/User.h"
#include "model/Stationlocal.h"
#include "model/RainGear.h"
#include "control/OperationResult.h"

// 客户端控制层：负责把业务流程串起来（不含服务端交互，后续再接）
class RainHubController {
public:
    // 借伞流程：
    // 1) 校验站点侧可用状态
    // 2) 取出雨具
    // 3) 扣押金，不足则回滚库存
    static OperationResult borrow(User &user, Stationlocal &station, const QString &gearId) {
        if (!station.is_gear_available(gearId)) {
            return OperationResult::fail(OperationResult::Code::Conflict, QStringLiteral("该雨具在此站点不可借用"));
        }

        // 尝试取出雨具
        std::unique_ptr<RainGear> gear = station.take_gear(gearId);
        if (!gear) {
            return OperationResult::fail(OperationResult::Code::NotFound, QStringLiteral("未找到指定雨具"));
        }

        const double deposit = gear->get_deposit();
        if (!user.deduct(deposit)) {
            // 余额不足：回滚库存
            station.add_gear(std::move(gear));
            return OperationResult::fail(OperationResult::Code::BadRequest, QStringLiteral("余额不足，无法支付押金"));
        }

        // 成功：此时雨具已从站点库存移除，押金已扣
        // 将雨具交给用户持有（独占所有权）
        user.take_gear(std::move(gear));
        return OperationResult::ok(QStringLiteral("借用成功"));
    }

    // 归还流程：
    // 1) 雨具入库
    // 2) 押金退回
    static OperationResult give_back(User &user, Stationlocal &station) {
        // 从用户处取回独占所有权
        std::unique_ptr<RainGear> gear = user.return_gear();
        if (!gear) {
            return OperationResult::fail(OperationResult::Code::BadRequest, QStringLiteral("当前用户无可归还雨具"));
        }
        double deposit = gear->get_deposit();
        station.add_gear(std::move(gear));
        user.recharge(deposit);
        return OperationResult::ok(QStringLiteral("归还成功"));
    }

    // 客户端侧报修/故障：仅做站点的不可用标记（服务端流程后续接入）
    static OperationResult report_damage(Stationlocal &station, const QString &gearId) {
        station.mark_unavailable(gearId);
        return OperationResult::ok(QStringLiteral("已标记为不可用"));
    }

    // 取消不可用标记（例如维修完毕）
    static OperationResult repair_done(Stationlocal &station, const QString &gearId) {
        station.mark_available(gearId);
        return OperationResult::ok(QStringLiteral("已恢复可用"));
    }
};
