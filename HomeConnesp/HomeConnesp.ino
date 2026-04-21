#define PIR_SENSOR 14;
#define REED_SENSOR 15;

void Setup(){
    pinMode(PIR_SENSOR, INPUT);
    pinMode(PIR_SENSOR, INPUT);
    pinMode(LED_BUILTIN, OUTPUT); // Led padrão do Esp

    Serial.begin(115200);

    //xTaskCreatePinnedToCore(funcao, nome, stackSize, parametros, prioridade, handler, core);
    xTaskCreatePinnedToCore(loop0, "LED", 1024, NULL, 1, NULL, 0); // Core 0
    xTaskCreatePinnedToCore(loop1, "Sensor", 1024, NULL, 1, NULL, 1); // Core 1
}

void loop0(void *pvParameters) {
    while(true) {
        digitalWrite(LED_BUILTIN, HIGH); // Liga o LED
        delay(2000);   
        digitalWrite(LED_BUILTIN, LOW);
        delay(2000);
    }
}
void loop1(void *pvParameters) {
    while(true) {
        bool motionDetected = digitalRead(PIR_SENSOR);
        Serial.println("Status pir: " + motionDetected);
        printMessage();
        delay(5000);
    }
}

void loop() { // Main loop does nothing }