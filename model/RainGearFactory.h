#pragma once

#include <memory>

#include"GlobalEnum.hpp"
#include"RainGear.h"
#include"RainGear_subclasses.hpp"

class RainGearFactory {
public:
    // 静态工厂方法，根据类型创建对应的雨具实例
    static std::unique_ptr<RainGear> create_rain_gear(GearType type, const QString &id) {
        switch (type) {
            case GearType::StandardPlastic:
                return std::make_unique<PlasticUmbrella>(id);
            case GearType::PremiumWindproof:
                return std::make_unique<HighQualityUmbrella>(id);
            case GearType::Sunshade:
                return std::make_unique<SunshadeUmbrella>(id);
            case GearType::Raincoat:
                return std::make_unique<Raincoat>(id);
            default:
                return nullptr; // 未知类型返回空指针
        }
    }
};

