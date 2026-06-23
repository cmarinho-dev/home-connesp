#include "Config.h"
#include <Preferences.h>
#include <ctype.h>

static Preferences prefs;

static uint32_t sensorInterval   = 2000;
static bool     lightSleepEn     = false;

// Default Wi-Fi tried after flashing. If it fails, the web server starts in AP mode.
static const char* DEFAULT_WIFI_SSID = "rinho.dev";
static const char* DEFAULT_WIFI_PASS = "11111001";

static String wifiSSID = DEFAULT_WIFI_SSID;
static String wifiPass = DEFAULT_WIFI_PASS;

static String jsonEscape(const String& value) {
    String out;
    out.reserve(value.length() + 8);
    for (size_t i = 0; i < value.length(); i++) {
        char c = value.charAt(i);
        if (c == '"' || c == '\\') {
            out += '\\';
            out += c;
        } else if (c == '\n') {
            out += "\\n";
        } else if (c == '\r') {
            out += "\\r";
        } else if (c == '\t') {
            out += "\\t";
        } else {
            out += c;
        }
    }
    return out;
}

static bool extractJsonString(const String& json, const char* key, String& value) {
    String token = "\"" + String(key) + "\":";
    int idx = json.indexOf(token);
    if (idx < 0) return false;

    int start = json.indexOf('"', idx + token.length());
    if (start < 0) return false;

    String out;
    bool escaped = false;
    for (int i = start + 1; i < (int)json.length(); i++) {
        char c = json.charAt(i);
        if (escaped) {
            switch (c) {
                case 'n': out += '\n'; break;
                case 'r': out += '\r'; break;
                case 't': out += '\t'; break;
                default:  out += c;    break;
            }
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else if (c == '"') {
            value = out;
            return true;
        } else {
            out += c;
        }
    }
    return false;
}

static bool extractJsonBool(const String& json, const char* key, bool& value) {
    String token = "\"" + String(key) + "\":";
    int idx = json.indexOf(token);
    if (idx < 0) return false;

    int start = idx + token.length();
    while (start < (int)json.length() && isspace((unsigned char)json.charAt(start))) {
        start++;
    }

    if (json.substring(start, start + 4) == "true") {
        value = true;
        return true;
    }
    if (json.substring(start, start + 5) == "false") {
        value = false;
        return true;
    }
    return false;
}

void Config::init() {
    prefs.begin("homeconn", false);
    sensorInterval = prefs.getUInt("sens_ms", 2000);
    lightSleepEn   = prefs.getBool("lsleep",  false);
    wifiSSID       = prefs.getString("wifi_ssid", DEFAULT_WIFI_SSID);
    wifiPass       = prefs.getString("wifi_pass", DEFAULT_WIFI_PASS);
}

void Config::save() {
    prefs.putUInt("sens_ms", sensorInterval);
    prefs.putBool("lsleep",  lightSleepEn);
    prefs.putString("wifi_ssid", wifiSSID);
    prefs.putString("wifi_pass", wifiPass);
}

uint32_t Config::getSensorInterval()           { return sensorInterval; }
void     Config::setSensorInterval(uint32_t ms){ sensorInterval = ms; }

bool Config::getLightSleepEnabled()            { return lightSleepEn; }
void Config::setLightSleepEnabled(bool en)     { lightSleepEn = en; }

String Config::getSSID()                       { return wifiSSID; }
String Config::getPassword()                   { return wifiPass; }

void Config::setWiFiCredentials(const String& ssid, const String& password) {
    wifiSSID = ssid;
    wifiSSID.trim();
    wifiPass = password;
}

String Config::toJSON() {
    String j = "{";
    j += "\"sensor_interval\":" + String(sensorInterval) + ",";
    j += "\"light_sleep\":"     + String(lightSleepEn ? "true" : "false") + ",";
    j += "\"wifi_ssid\":\""     + jsonEscape(wifiSSID) + "\"";
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
    extractJsonBool(json, "light_sleep", lightSleepEn);

    String ssid;
    if (extractJsonString(json, "wifi_ssid", ssid)) {
        ssid.trim();
        if (ssid.length() > 0) wifiSSID = ssid;
    }

    String password;
    if (extractJsonString(json, "wifi_password", password)) {
        wifiPass = password;
    }
}
