/*
这是一个静态工具类，将上面的英文枚举翻译成中文，供界面 (UI) 显示使用。
*/
#include <QString>
#include "GlobalDefs.h"

class StationUtils {
public:
    // [核心功能] 输入英文枚举，返回中文名称
    static QString getChineseName(StationCode code) {
        switch (code) {
            case StationCode::Wende:     return "文德楼";
            case StationCode::Mingde:    return "明德楼";
            case StationCode::Library:   return "图书馆";
            case StationCode::Changwang: return "长望楼";
            case StationCode::Oufang:    return "藕舫楼";
            case StationCode::Beichen:   return "北辰楼";
            case StationCode::Dorm1:     return "西苑宿舍楼1";
            case StationCode::Dorm2:     return "西苑宿舍楼2";
            case StationCode::Dorm3:     return "西苑宿舍楼3";
            case StationCode::Gym:       return "体育馆";
            case StationCode::Admin:     return "行政楼";
            default:                     return "未知站点";
        }
    }
    
    // [核心功能] 输入英文枚举，返回站点描述 (可选)
    static QString getDescription(StationCode code) {
        if (code == StationCode::Library) return "学习氛围浓厚，雨伞需求量大";
        if (code == StationCode::Gym)     return "运动结束后常有借伞需求";
        return "普通教学与办公区域";
    }
};