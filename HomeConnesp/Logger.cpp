#include "Logger.h"
#include <LittleFS.h>

#define LOG_FILE     "/logs.csv"
#define MAX_RAM_LOGS 200
#define MAX_AGE_MS   (24UL * 3600UL * 1000UL)

static LogEntry ramBuf[MAX_RAM_LOGS];
static int      ramHead  = 0;
static int      ramCount = 0;
static SemaphoreHandle_t logMux = NULL;

static const char* levelStr(LogLevel l) {
    switch (l) {
        case LOG_INFO:  return "INFO";
        case LOG_WARN:  return "WARN";
        case LOG_ERROR: return "ERROR";
    }
    return "INFO";
}

void Logger::init() {
    logMux = xSemaphoreCreateMutex();
    // Load persisted logs into RAM
    if (!LittleFS.exists(LOG_FILE)) return;
    File f = LittleFS.open(LOG_FILE, "r");
    if (!f) return;
    while (f.available() && ramCount < MAX_RAM_LOGS) {
        String line = f.readStringUntil('\n');
        if (line.length() < 5) continue;
        // Format: timestamp,LEVEL,message
        int c1 = line.indexOf(',');
        int c2 = line.indexOf(',', c1 + 1);
        if (c1 < 0 || c2 < 0) continue;
        LogEntry& e = ramBuf[ramHead];
        e.timestamp = line.substring(0, c1).toInt();
        String lv   = line.substring(c1 + 1, c2);
        e.level     = (lv == "WARN") ? LOG_WARN : (lv == "ERROR") ? LOG_ERROR : LOG_INFO;
        line.substring(c2 + 1).toCharArray(e.message, sizeof(e.message));
        ramHead  = (ramHead + 1) % MAX_RAM_LOGS;
        ramCount = min(ramCount + 1, MAX_RAM_LOGS);
    }
    f.close();
}

void Logger::log(LogLevel level, const char* msg) {
    if (!logMux) return;
    xSemaphoreTake(logMux, portMAX_DELAY);
    LogEntry& e = ramBuf[ramHead];
    e.timestamp = millis();
    e.level     = level;
    strncpy(e.message, msg, sizeof(e.message) - 1);
    e.message[sizeof(e.message) - 1] = '\0';
    ramHead  = (ramHead + 1) % MAX_RAM_LOGS;
    if (ramCount < MAX_RAM_LOGS) ramCount++;
    xSemaphoreGive(logMux);
    Serial.printf("[%s] %s\n", levelStr(level), msg);
}

void Logger::log(LogLevel level, const String& msg) {
    log(level, msg.c_str());
}

void Logger::flush() {
    if (!logMux) return;
    xSemaphoreTake(logMux, portMAX_DELAY);
    File f = LittleFS.open(LOG_FILE, "w");
    if (f) {
        uint32_t now = millis();
        int start = (ramHead - ramCount + MAX_RAM_LOGS) % MAX_RAM_LOGS;
        for (int i = 0; i < ramCount; i++) {
            LogEntry& e = ramBuf[(start + i) % MAX_RAM_LOGS];
            if (now - e.timestamp < MAX_AGE_MS) {
                f.printf("%u,%s,%s\n", e.timestamp, levelStr(e.level), e.message);
            }
        }
        f.close();
    }
    xSemaphoreGive(logMux);
}

void Logger::purgeOld() {
    if (!logMux) return;
    xSemaphoreTake(logMux, portMAX_DELAY);
    uint32_t now = millis();
    // Rebuild buffer keeping only entries < 24h
    LogEntry tmp[MAX_RAM_LOGS];
    int newCount = 0;
    int start = (ramHead - ramCount + MAX_RAM_LOGS) % MAX_RAM_LOGS;
    for (int i = 0; i < ramCount; i++) {
        LogEntry& e = ramBuf[(start + i) % MAX_RAM_LOGS];
        if (now - e.timestamp < MAX_AGE_MS) {
            tmp[newCount++] = e;
        }
    }
    memcpy(ramBuf, tmp, sizeof(LogEntry) * newCount);
    ramCount = newCount;
    ramHead  = newCount % MAX_RAM_LOGS;
    xSemaphoreGive(logMux);
}

String Logger::exportCSV() {
    String out = "timestamp_ms,uptime_s,level,message\n";
    if (!logMux) return out;
    xSemaphoreTake(logMux, portMAX_DELAY);
    int start = (ramHead - ramCount + MAX_RAM_LOGS) % MAX_RAM_LOGS;
    for (int i = 0; i < ramCount; i++) {
        LogEntry& e = ramBuf[(start + i) % MAX_RAM_LOGS];
        out += String(e.timestamp) + "," +
               String(e.timestamp / 1000) + "," +
               levelStr(e.level) + "," +
               String(e.message) + "\n";
    }
    xSemaphoreGive(logMux);
    return out;
}

void Logger::clear() {
    if (!logMux) return;
    xSemaphoreTake(logMux, portMAX_DELAY);
    ramCount = 0;
    ramHead  = 0;
    LittleFS.remove(LOG_FILE);
    xSemaphoreGive(logMux);
}

int Logger::count() { return ramCount; }

LogEntry Logger::get(int index) {
    int start = (ramHead - ramCount + MAX_RAM_LOGS) % MAX_RAM_LOGS;
    return ramBuf[(start + index) % MAX_RAM_LOGS];
}

String Logger::toJSON(int offset, int limit) {
    if (!logMux) return "[]";
    xSemaphoreTake(logMux, portMAX_DELAY);
    String out = "[";
    int start = (ramHead - ramCount + MAX_RAM_LOGS) % MAX_RAM_LOGS;
    int end   = min(offset + limit, ramCount);
    for (int i = offset; i < end; i++) {
        LogEntry& e = ramBuf[(start + i) % MAX_RAM_LOGS];
        if (i > offset) out += ",";
        out += "{\"ts\":" + String(e.timestamp) +
               ",\"level\":\"" + levelStr(e.level) + "\"" +
               ",\"msg\":\"" + String(e.message) + "\"}";
    }
    out += "]";
    xSemaphoreGive(logMux);
    return out;
}
