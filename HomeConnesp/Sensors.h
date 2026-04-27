#pragma once
#include <Arduino.h>

namespace Sensors {
    void init(uint8_t pirPin, uint8_t reedPin);

    bool getMotion();
    bool getDoor();       // true = open

    void setMotion(bool v);
    void setDoor(bool v);

    uint32_t getLastMotionTime();
    uint32_t getLastDoorTime();
}
