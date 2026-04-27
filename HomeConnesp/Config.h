#pragma once
#include <Arduino.h>

namespace Config {
    void init();
    void save();

    uint32_t getSensorInterval();
    void     setSensorInterval(uint32_t ms);

    bool getLightSleepEnabled();
    void setLightSleepEnabled(bool en);

    String getSSID();
    String getPassword();
    void   setWiFi(const String& ssid, const String& pass);

    String toJSON();
    void   fromJSON(const String& json);
}
