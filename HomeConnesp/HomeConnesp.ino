#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <esp_sleep.h>
#include "Config.h"
#include "Logger.h"
#include "Sensors.h"
#include "Actuators.h"
#include "Performance.h"
#include "WebServer.h"

// ── Pin map ───────────────────────────────────────────────────────────────────
#define PIN_PIR    5
#define PIN_REED   15
#define PIN_RELAY  26
#define PIN_SERVO  27

// ── Task handles ──────────────────────────────────────────────────────────────
static TaskHandle_t hSensors   = NULL;
static TaskHandle_t hActuators = NULL;
static TaskHandle_t hPerf      = NULL;
static TaskHandle_t hLogger    = NULL;
static TaskHandle_t hSleep     = NULL;

// ── Interrupt flags (set in ISR, cleared in task) ─────────────────────────────
static volatile bool isrPIR  = false;
static volatile bool isrReed = false;

void IRAM_ATTR onPIR()  { isrPIR  = true; }
void IRAM_ATTR onReed() { isrReed = true; }

// ── Tasks ─────────────────────────────────────────────────────────────────────

// Reads sensors at configured interval; handles interrupt flags immediately
static void taskSensors(void* pv) {
    while (true) {
        uint32_t t0 = micros();

        if (isrPIR) {
            isrPIR = false;
            bool motion = digitalRead(PIN_PIR);
            Sensors::setMotion(motion);
            Logger::log(motion ? LOG_INFO : LOG_INFO,
                        motion ? "PIR: movimento detectado" : "PIR: sem movimento");
        }

        if (isrReed) {
            isrReed = false;
            bool open = !digitalRead(PIN_REED); // reed NC: LOW = closed, HIGH = open
            Sensors::setDoor(open);
            Logger::log(open ? LOG_WARN : LOG_INFO,
                        open ? "Reed: porta aberta" : "Reed: porta fechada");
        }

        Performance::recordFunctionTime("taskSensors", micros() - t0);
        vTaskDelay(pdMS_TO_TICKS(Config::getSensorInterval()));
    }
}

// Runs actuator auto-logic at 100 ms cadence
static void taskActuators(void* pv) {
    while (true) {
        uint32_t t0 = micros();
        Actuators::update();
        Performance::recordFunctionTime("taskActuators", micros() - t0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Collects perf metrics every 1 s (core 0 so it doesn't starve app tasks)
static void taskPerformance(void* pv) {
    while (true) {
        uint32_t t0 = micros();
        Performance::update();
        Performance::recordFunctionTime("taskPerformance", micros() - t0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Flushes logs to LittleFS every 10 s and purges entries older than 24 h
static void taskLogger(void* pv) {
    while (true) {
        uint32_t t0 = micros();
        Logger::flush();
        Logger::purgeOld();
        Performance::recordFunctionTime("taskLogger", micros() - t0);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

// Monitors WiFi and triggers light sleep when system is idle
static void taskSleepMonitor(void* pv) {
    static uint32_t lastActivity = 0;

    while (true) {
        uint32_t t0 = micros();

        // WiFi watchdog
        if (WiFi.status() != WL_CONNECTED) {
            Logger::log(LOG_WARN, "WiFi perdido, reconectando...");
            WiFi.reconnect();
        }

        // Light sleep: only when enabled and no motion for 5 min
        if (Config::getLightSleepEnabled()) {
            bool active = Sensors::getMotion() || Sensors::getDoor();
            if (active) lastActivity = millis();

            if (millis() - lastActivity > 300000) { // 5 min idle
                Logger::log(LOG_INFO, "Sistema ocioso: entrando em light sleep (1s)");
                Logger::flush();
                esp_sleep_enable_timer_wakeup(1000000ULL); // 1 s
                esp_light_sleep_start();
                Logger::log(LOG_INFO, "Acordando do light sleep");
                lastActivity = millis();
            }
        }

        Performance::recordFunctionTime("taskSleepMonitor", micros() - t0);
        vTaskDelay(pdMS_TO_TICKS(30000)); // check every 30 s
    }
}

// ── Setup ─────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);

    if (!LittleFS.begin(true)) {
        Serial.println("[ERRO] LittleFS nao montado");
    }

    Config::init();
    Logger::init();
    Performance::init();
    Sensors::init(PIN_PIR, PIN_REED);
    Actuators::init(PIN_RELAY, PIN_SERVO);

    // Hardware interrupts
    attachInterrupt(digitalPinToInterrupt(PIN_PIR),  onPIR,  CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_REED), onReed, CHANGE);

    // Web server (blocks briefly while connecting WiFi)
    initWebServer();

    Logger::log(LOG_INFO, "HomeConnesp v1.0.0 iniciado");

    // ── Create FreeRTOS tasks ─────────────────────────────────────────────────
    //                         name           stack   param  prio  handle   core
    xTaskCreatePinnedToCore(taskSensors,     "Sensors",   4096, NULL, 3, &hSensors,   1);
    xTaskCreatePinnedToCore(taskActuators,   "Actuators", 2048, NULL, 2, &hActuators, 1);
    xTaskCreatePinnedToCore(taskPerformance, "Perf",      4096, NULL, 1, &hPerf,      0);
    xTaskCreatePinnedToCore(taskLogger,      "Logger",    4096, NULL, 1, &hLogger,    0);
    xTaskCreatePinnedToCore(taskSleepMonitor,"SleepMon",  2048, NULL, 1, &hSleep,     0);
}

// loop() runs in its own Arduino task — keep it empty; all logic is in tasks
void loop() {
    vTaskDelay(pdMS_TO_TICKS(10000));
}
