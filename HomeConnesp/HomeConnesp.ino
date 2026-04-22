#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "WebServer.h"

#define PIR_SENSOR 5
#define REED_SENSOR 15
#define ESP_LED 2

bool motionStatus = false;  // ← tirar o extern, definir aqui

void setup(){
    Serial.begin(115200);

    pinMode(PIR_SENSOR, INPUT);
    pinMode(REED_SENSOR, INPUT);
    pinMode(ESP_LED, OUTPUT);

    xTaskCreatePinnedToCore(loop0, "LED", 1024, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(loop1, "Sensor", 1024, NULL, 1, NULL, 1);

    initWebServer();
}

void loop0(void *pvParameters) {
    while(true) {
        digitalWrite(ESP_LED, HIGH);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        digitalWrite(ESP_LED, LOW);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void loop1(void *pvParameters) {
    while(true) {
        motionStatus = digitalRead(PIR_SENSOR);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void loop() {}