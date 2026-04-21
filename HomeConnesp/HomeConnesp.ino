//#include <WiFi.h>
//#include <ESPAsyncWebServer.h>
//#include <NTPClient.h>
//#include <WiFiUdp.h>
//#include <Preferences.h>

#define PIR_SENSOR 5
#define REED_SENSOR 15
#define ESP_LED 2

void setup(){
    Serial.begin(115200);

    pinMode(PIR_SENSOR, INPUT);
    pinMode(REED_SENSOR, INPUT);
    pinMode(ESP_LED, OUTPUT); // Led padrão do Esp

    //xTaskCreatePinnedToCore(funcao, nome, stackSize, parametros, prioridade, handler, core);
    xTaskCreatePinnedToCore(loop0, "LED", 1024, NULL, 1, NULL, 0); // Core 0
    xTaskCreatePinnedToCore(loop1, "Sensor", 1024, NULL, 1, NULL, 1); // Core 1
}

void loop0(void *pvParameters) {
    while(true) {
        digitalWrite(ESP_LED, HIGH); // Liga o LED
        delay(2000);   
        digitalWrite(ESP_LED, LOW);
        delay(2000);
    }
}
void loop1(void *pvParameters) {
    while(true) {
        bool motionDetected = digitalRead(PIR_SENSOR);
        printMessage(motionDetected);
        delay(5000);
    }
}

void loop() { 
    // Main loop does nothing 
}