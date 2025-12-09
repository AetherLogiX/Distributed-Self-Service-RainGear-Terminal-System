
#include <QList>
#include <QString>
#include <memory>

#include "GlobalEnum.hpp"
#include "RainGear.h"
#include "RainGearFactory.h"
#include "RainGear_subclasses.hpp"
#include "StationUtils.h"
#include "Stationlocal.h"

Stationlocal::Stationlocal(Station station, double posX, double posY)
    : station(station)
    , name(StationUtils::getChineseName(station))
    , posX(posX)
    , posY(posY)
{}

// 获取可用库存数量：过滤掉被站点标记为不可用的雨具
int Stationlocal::get_available_count() const
{
    int count = 0;
    for (const auto &gear : inventory) {
        if (!gear) continue;
        const QString gid = gear->get_id();
        if (!unavailable_gears.contains(gid)) {
            ++count;
        }
    }
    return count;
}

//添加雨具到库存
void Stationlocal::add_gear(std::unique_ptr<RainGear> gear)
{
    if (gear) {
        inventory.push_back(std::move(gear));
    }
}

//从库存中取出雨具
std::unique_ptr<RainGear> Stationlocal::take_gear(const QString &gearId)
{
    for (size_t i = 0; i < inventory.size(); ++i) {
        if (inventory[i] && inventory[i]->get_id() == gearId) {
            std::unique_ptr<RainGear> out = std::move(inventory[i]);
            // 删除该元素：用最后一个覆盖或直接 erase
            inventory.erase(inventory.begin() + static_cast<std::ptrdiff_t>(i));
            return out;
        }
    }
    return nullptr;
}

// 将某件雨具标记为不可用（如槽位故障或保修期内暂不可借）
void Stationlocal::mark_unavailable(const QString &gearId)
{
    unavailable_gears.insert(gearId);
}

// 恢复可用状态
void Stationlocal::mark_available(const QString &gearId)
{
    unavailable_gears.remove(gearId);
}

// 查询某件雨具在站点侧是否可用
bool Stationlocal::is_gear_available(const QString &gearId) const
{
    return !unavailable_gears.contains(gearId);
}

