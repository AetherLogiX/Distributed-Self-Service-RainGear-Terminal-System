#include "RainGear.h"

RainGear::RainGear(QString id, GearType type)
    : id(std::move(id))
    , type(type)
{}

RainGear::~RainGear() = default;

