#include "Config.h"
#include <Preferences.h>

static Preferences prefs;

static uint32_t sensorInterval   = 2000;
static bool     lightSleepEn     = false;
static String   wifiSSID         = "rinho.dev";
static String   wifiPass         = "10001000";

void Config::init() {
    prefs.begin("homeconn", false);
    sensorInterval = prefs.getUInt("sens_ms", 2000);
    lightSleepEn   = prefs.getBool("lsleep",  false);
    wifiSSID       = prefs.getString("ssid",  "rinho.dev");
    wifiPass       = prefs.getString("pass",  "10010001");
}

void Config::save() {
    prefs.putUInt("sens_ms", sensorInterval);
    prefs.putBool("lsleep",  lightSleepEn);
    prefs.putString("ssid",  wifiSSID);
    prefs.putString("pass",  wifiPass);
}

uint32_t Config::getSensorInterval()           { return sensorInterval; }
void     Config::setSensorInterval(uint32_t ms){ sensorInterval = ms; }

bool Config::getLightSleepEnabled()            { return lightSleepEn; }
void Config::setLightSleepEnabled(bool en)     { lightSleepEn = en; }

String Config::getSSID()                       { return wifiSSID; }
String Config::getPassword()                   { return wifiPass; }
void   Config::setWiFi(const String& s, const String& p){ wifiSSID = s; wifiPass = p; }

String Config::toJSON() {
    String j = "{";
    j += "\"sensor_interval\":" + String(sensorInterval) + ",";
    j += "\"light_sleep\":"     + String(lightSleepEn ? "true" : "false") + ",";
    j += "\"ssid\":\""          + wifiSSID + "\"";
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
    // ssid
    idx = json.indexOf("\"ssid\":");
    if (idx >= 0) {
        int s = json.indexOf('"', idx + 7) + 1;
        int e = json.indexOf('"', s);
        wifiSSID = json.substring(s, e);
    }
    // password
    idx = json.indexOf("\"password\":");
    if (idx >= 0) {
        int s = json.indexOf('"', idx + 11) + 1;
        int e = json.indexOf('"', s);
        wifiPass = json.substring(s, e);
    }
}
