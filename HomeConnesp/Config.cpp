#include "Config.h"
#include <Preferences.h>

static Preferences prefs;

static uint32_t sensorInterval   = 2000;
static bool     lightSleepEn     = false;
static const char* WIFI_SSID     = "rinho.dev";
static const char* WIFI_PASS     = "11111001";

void Config::init() {
    prefs.begin("homeconn", false);
    sensorInterval = prefs.getUInt("sens_ms", 2000);
    lightSleepEn   = prefs.getBool("lsleep",  false);
}

void Config::save() {
    prefs.putUInt("sens_ms", sensorInterval);
    prefs.putBool("lsleep",  lightSleepEn);
}

uint32_t Config::getSensorInterval()           { return sensorInterval; }
void     Config::setSensorInterval(uint32_t ms){ sensorInterval = ms; }

bool Config::getLightSleepEnabled()            { return lightSleepEn; }
void Config::setLightSleepEnabled(bool en)     { lightSleepEn = en; }

String Config::getSSID()                       { return WIFI_SSID; }
String Config::getPassword()                   { return WIFI_PASS; }

String Config::toJSON() {
    String j = "{";
    j += "\"sensor_interval\":" + String(sensorInterval) + ",";
    j += "\"light_sleep\":"     + String(lightSleepEn ? "true" : "false");
    j += "}";
    return j;
}

void Config::fromJSON(const String& json) {
    // sensor_interval
    int idx = json.indexOf("\"sensor_interval\":");
    if (idx >= 0) {
        int start = json.indexOf(':', idx) + 1;
        int end   = json.indexOf(',', start);
        if (end < 0) end = json.indexOf('}', start);
        sensorInterval = json.substring(start, end).toInt();
    }
    // light_sleep
    idx = json.indexOf("\"light_sleep\":");
    if (idx >= 0) {
        lightSleepEn = json.indexOf("true", idx) != -1;
    }
}
