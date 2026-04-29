#pragma once
#include <Arduino.h>

enum LogLevel { LOG_INFO, LOG_WARN, LOG_ERROR };

struct LogEntry {
    uint32_t timestamp; // millis
    LogLevel level;
    char     message[96];
};

namespace Logger {
    void init();
    void log(LogLevel level, const char* msg);
    void log(LogLevel level, const String& msg);
    void flush();              // persist to LittleFS
    void purgeOld();           // remove entries > 24h
    String exportCSV();
    void   clear();

    int         count();
    LogEntry    get(int index);
    String      toJSON(int offset = 0, int limit = 100);
}
