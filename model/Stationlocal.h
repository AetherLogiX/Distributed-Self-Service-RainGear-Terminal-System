/*
定义了客户端用来表示借还站点的类 Stationlocal。
posX 和 posY 表示站点的地理位置坐标，inventory 则维护了该站点当前库存的雨具列表。
该类提供了添加雨具到库存和从库存中取出雨具的方法，以及查询当前库存数量的方法。
*/

#pragma once

#include <QSet>
#include <QString>
#include <memory>
#include <vector>

#include "GlobalEnum.hpp"
#include "RainGear.h"
#include "RainGearFactory.h"
#include "RainGear_subclasses.hpp"

// 站点实体，维护库存与位置信息。
class Stationlocal {
public:
    Stationlocal(Station station, double posX, double posY);

    Station get_station() const { return station; }
    const QString& get_name() const { return name; }
    double get_posX() const { return posX; }
    double get_posY() const { return posY; }

    // 库存总数量与可用数量（可用数量会过滤不可用标记）
    int get_inventory_count() const { return inventory.size(); }
    int get_available_count() const;

    void add_gear(std::unique_ptr<RainGear> gear); //添加雨具到库存（独占所有权）
    std::unique_ptr<RainGear> take_gear(const QString &gearId); //从库存中取出雨具（转移所有权）

    // 站点侧管理“不可用”状态（例如保修/故障上报后，服务端将该槽位或该伞置不可用）
    //用于服务器标记某伞不可用/恢复可用
    void mark_unavailable(const QString &gearId);
    void mark_available(const QString &gearId);
    bool is_gear_available(const QString &gearId) const;
 
private:
    Station station;
    QString name;
    double posX;
    double posY;
    std::vector<std::unique_ptr<RainGear>> inventory;  //库存雨具列表（独占所有权）
    QSet<QString> unavailable_gears;              //站点侧标记为不可用的雨具ID集合
};
