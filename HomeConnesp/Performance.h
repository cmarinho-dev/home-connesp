#pragma once
#include <Arduino.h>

#define MAX_FUNC_TRACKED 10

struct FuncStat {
    char     name[32];
    uint32_t lastUs;
    uint32_t minUs;
    uint32_t maxUs;
    uint32_t avgUs;
    uint32_t callCount;
};

struct TaskStat {
    char     name[16];
    uint32_t stackWatermark; // bytes free
    uint8_t  priority;
    char     state;          // 'R'unning,'B'locked,'S'uspended,'D'eleted
};

namespace Performance {
    void init();
    void update();   // call every second from a task

    void recordFunctionTime(const char* name, uint32_t us);

    float    getCpuLoad();       // 0–100 %
    uint32_t getHeapFree();
    uint32_t getHeapTotal();
    uint32_t getHeapMinFree();
    uint32_t getPsramFree();
    uint32_t getPsramTotal();
    uint32_t getFlashSize();
    uint32_t getSketchSize();
    uint32_t getUptime();        // seconds

    int       getFuncCount();
    FuncStat  getFunc(int i);

    int       getTaskCount();
    TaskStat  getTask(int i);

    String toJSON();
}
