#include "Performance.h"
#include <esp_heap_caps.h>

// CPU load estimation via idle counter task
static volatile uint32_t idleCounter   = 0;
static uint32_t          idleBaseline  = 0; // counts per second at 0% load
static float             cpuLoad       = 0.0f;

static FuncStat funcStats[MAX_FUNC_TRACKED];
static int      funcCount = 0;
static SemaphoreHandle_t perfMux = NULL;

// Idle counter task — lowest priority, yields constantly
static void taskIdleCounter(void* pv) {
    // Run for 1 second to calibrate baseline
    uint32_t start = millis();
    uint32_t cal   = 0;
    while (millis() - start < 1000) { cal++; }
    idleBaseline = cal;
    // Normal idle counting
    while (true) {
        idleCounter++;
        taskYIELD();
    }
}

void Performance::init() {
    perfMux = xSemaphoreCreateMutex();
    memset(funcStats, 0, sizeof(funcStats));
    // Calibration + idle counter at lowest priority on core 0
    xTaskCreatePinnedToCore(taskIdleCounter, "IdleCnt", 1024, NULL, 0, NULL, 0);
}

void Performance::update() {
    static uint32_t lastIdle = 0;
    static uint32_t lastMs   = 0;
    uint32_t now   = millis();
    uint32_t delta = now - lastMs;
    if (delta == 0) return;

    uint32_t curIdle  = idleCounter;
    uint32_t idleDiff = curIdle - lastIdle;
    lastIdle = curIdle;
    lastMs   = now;

    if (idleBaseline > 0) {
        float expectedIdle = idleBaseline * (delta / 1000.0f);
        float idleFrac     = (float)idleDiff / expectedIdle;
        idleFrac   = constrain(idleFrac, 0.0f, 1.0f);
        cpuLoad    = (1.0f - idleFrac) * 100.0f;
    }
}

void Performance::recordFunctionTime(const char* name, uint32_t us) {
    if (!perfMux) return;
    xSemaphoreTake(perfMux, portMAX_DELAY);
    // Find existing
    for (int i = 0; i < funcCount; i++) {
        if (strncmp(funcStats[i].name, name, 31) == 0) {
            funcStats[i].lastUs = us;
            if (funcStats[i].callCount == 0 || us < funcStats[i].minUs) funcStats[i].minUs = us;
            if (us > funcStats[i].maxUs) funcStats[i].maxUs = us;
            funcStats[i].avgUs = (funcStats[i].avgUs * funcStats[i].callCount + us) / (funcStats[i].callCount + 1);
            funcStats[i].callCount++;
            xSemaphoreGive(perfMux);
            return;
        }
    }
    // New entry
    if (funcCount < MAX_FUNC_TRACKED) {
        strncpy(funcStats[funcCount].name, name, 31);
        funcStats[funcCount].lastUs    = us;
        funcStats[funcCount].minUs     = us;
        funcStats[funcCount].maxUs     = us;
        funcStats[funcCount].avgUs     = us;
        funcStats[funcCount].callCount = 1;
        funcCount++;
    }
    xSemaphoreGive(perfMux);
}

float    Performance::getCpuLoad()    { return cpuLoad; }
uint32_t Performance::getHeapFree()   { return esp_get_free_heap_size(); }
uint32_t Performance::getHeapTotal()  { return heap_caps_get_total_size(MALLOC_CAP_DEFAULT); }
uint32_t Performance::getHeapMinFree(){ return esp_get_minimum_free_heap_size(); }
uint32_t Performance::getPsramFree()  { return heap_caps_get_free_size(MALLOC_CAP_SPIRAM); }
uint32_t Performance::getPsramTotal() { return heap_caps_get_total_size(MALLOC_CAP_SPIRAM); }
uint32_t Performance::getFlashSize()  { return ESP.getFlashChipSize(); }
uint32_t Performance::getSketchSize() { return ESP.getSketchSize(); }
uint32_t Performance::getUptime()     { return millis() / 1000; }

int      Performance::getFuncCount()  { return funcCount; }
FuncStat Performance::getFunc(int i)  { return funcStats[i]; }

int Performance::getTaskCount() { return (int)uxTaskGetNumberOfTasks(); }

TaskStat Performance::getTask(int i) {
    TaskStat ts;
    memset(&ts, 0, sizeof(ts));
    // Enumerate tasks via FreeRTOS snapshot
    UBaseType_t n = uxTaskGetNumberOfTasks();
    TaskStatus_t* buf = (TaskStatus_t*)pvPortMalloc(n * sizeof(TaskStatus_t));
    if (!buf) return ts;
    uint32_t total;
    UBaseType_t got = uxTaskGetSystemState(buf, n, &total);
    if ((int)i < (int)got) {
        strncpy(ts.name, buf[i].pcTaskName, 15);
        ts.stackWatermark = buf[i].usStackHighWaterMark * sizeof(StackType_t);
        ts.priority       = buf[i].uxCurrentPriority;
        switch (buf[i].eCurrentState) {
            case eRunning:   ts.state = 'R'; break;
            case eBlocked:   ts.state = 'B'; break;
            case eSuspended: ts.state = 'S'; break;
            case eDeleted:   ts.state = 'D'; break;
            default:         ts.state = '?'; break;
        }
    }
    vPortFree(buf);
    return ts;
}

String Performance::toJSON() {
    String j = "{";
    j += "\"cpu\":"          + String(cpuLoad, 1) + ",";
    j += "\"heap_free\":"    + String(getHeapFree())    + ",";
    j += "\"heap_total\":"   + String(getHeapTotal())   + ",";
    j += "\"heap_min\":"     + String(getHeapMinFree()) + ",";
    j += "\"psram_free\":"   + String(getPsramFree())   + ",";
    j += "\"psram_total\":"  + String(getPsramTotal())  + ",";
    j += "\"flash_size\":"   + String(getFlashSize())   + ",";
    j += "\"sketch_size\":"  + String(getSketchSize())  + ",";
    j += "\"uptime\":"       + String(getUptime())      + ",";
    // Function times
    j += "\"funcs\":[";
    if (perfMux) xSemaphoreTake(perfMux, portMAX_DELAY);
    for (int i = 0; i < funcCount; i++) {
        if (i) j += ",";
        j += "{\"name\":\"" + String(funcStats[i].name) + "\"" +
             ",\"last\":"   + String(funcStats[i].lastUs)  +
             ",\"min\":"    + String(funcStats[i].minUs)   +
             ",\"max\":"    + String(funcStats[i].maxUs)   +
             ",\"avg\":"    + String(funcStats[i].avgUs)   +
             ",\"calls\":"  + String(funcStats[i].callCount) + "}";
    }
    if (perfMux) xSemaphoreGive(perfMux);
    j += "],";
    // Tasks
    j += "\"tasks\":[";
    UBaseType_t n = uxTaskGetNumberOfTasks();
    TaskStatus_t* buf = (TaskStatus_t*)pvPortMalloc(n * sizeof(TaskStatus_t));
    if (buf) {
        uint32_t total;
        UBaseType_t got = uxTaskGetSystemState(buf, n, &total);
        for (UBaseType_t i = 0; i < got; i++) {
            if (i) j += ",";
            char st = '?';
            switch (buf[i].eCurrentState) {
                case eRunning:   st = 'R'; break;
                case eBlocked:   st = 'B'; break;
                case eSuspended: st = 'S'; break;
                case eDeleted:   st = 'D'; break;
                default: break;
            }
            j += "{\"name\":\"" + String(buf[i].pcTaskName) + "\"" +
                 ",\"prio\":"   + String(buf[i].uxCurrentPriority) +
                 ",\"stack\":"  + String(buf[i].usStackHighWaterMark * 4) +
                 ",\"state\":\"" + String(st) + "\"}";
        }
        vPortFree(buf);
    }
    j += "]}";
    return j;
}
