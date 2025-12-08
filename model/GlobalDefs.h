#include <QObject> // 为了使用 Q_ENUM 等特性

// 1. 雨具类型枚举 (对应数据库 t_rain_gear.type_id)
enum class GearType {
    Unknown = 0,
    StandardPlastic = 1,    // 普通塑料伞
    PremiumWindproof = 2,   // 高质量抗风伞
    Sunshade = 3,           // 专用遮阳伞
    Raincoat = 4            // 雨衣
};

// 2. 站点枚举 (代码中使用这些英文名，避免乱码)
enum class StationCode {
    Wende,      // 文德楼
    Mingde,     // 明德楼
    Library,    // 图书馆
    Changwang,  // 长望楼
    Oufang,     // 藕舫楼
    Beichen,    // 北辰楼
    Dorm1,      // 西苑宿舍楼1
    Dorm2,      // 西苑宿舍楼2
    Dorm3,      // 西苑宿舍楼3
    Gym,        // 体育馆
    Admin       // 行政楼
};