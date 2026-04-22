#include "WebServer.h"

// todo o seu código do WebServer.ino aqui
// Dados do WiFi
const char* ssid = "rinho.dev";
const char* password = "10010001";

AsyncWebServer server(80);

// Variável global (compartilhada com sensores)
extern bool motionStatus;

// HTML com Tailwind CDN
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<title>ESP32 Dashboard</title>

<script src="https://cdn.tailwindcss.com"></script>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

</head>

<body class="bg-gray-900 text-white min-h-screen p-6">

<h1 class="text-4xl font-bold mb-6 text-center">📡 Dashboard ESP32</h1>

<div class="grid grid-cols-1 md:grid-cols-2 gap-6">

    <!-- GRÁFICO PERFORMANCE -->
    <div class="bg-gray-800 p-6 rounded-2xl shadow-xl">
        <h2 class="text-xl mb-4">📊 Performance do Sistema</h2>
        <canvas id="performanceChart"></canvas>
    </div>

    <!-- GRÁFICO PRESENÇA -->
    <div class="bg-gray-800 p-6 rounded-2xl shadow-xl">
        <h2 class="text-xl mb-4">👁️ Presença (PIR)</h2>
        <canvas id="presenceChart"></canvas>
    </div>

</div>

<!-- LOG -->
<div class="bg-gray-800 mt-6 p-6 rounded-2xl shadow-xl">
    <h2 class="text-xl mb-4">📝 Registro de Eventos</h2>
    <ul id="log" class="space-y-2 max-h-60 overflow-y-auto"></ul>
</div>

<script>

// =======================
// 📊 GRÁFICO PERFORMANCE (FAKE)
// =======================
const perfCtx = document.getElementById('performanceChart');

const performanceChart = new Chart(perfCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Uso (%)',
            data: [],
            borderColor: '#3b82f6',
            tension: 0.4
        }]
    }
});

// =======================
// 👁️ GRÁFICO PRESENÇA
// =======================
const presenceCtx = document.getElementById('presenceChart');

const presenceChart = new Chart(presenceCtx, {
    type: 'bar',
    data: {
        labels: [],
        datasets: [{
            label: 'Movimento',
            data: [],
            backgroundColor: '#ef4444'
        }]
    }
});

// =======================
// 📝 LOG
// =======================
const logList = document.getElementById("log");

function addLog(message) {
    const li = document.createElement("li");
    li.className = "bg-gray-700 p-2 rounded";
    li.textContent = message;
    logList.prepend(li);
}

// =======================
// 🔄 SIMULAÇÃO + FETCH PIR
// =======================
setInterval(() => {

    // Tempo
    const now = new Date().toLocaleTimeString();

    // Fake performance
    const fakeValue = Math.floor(Math.random() * 100);

    performanceChart.data.labels.push(now);
    performanceChart.data.datasets[0].data.push(fakeValue);

    if (performanceChart.data.labels.length > 10) {
        performanceChart.data.labels.shift();
        performanceChart.data.datasets[0].data.shift();
    }

    performanceChart.update();

    // Buscar PIR real do ESP32
    fetch("/status")
    .then(res => res.text())
    .then(data => {

        const detected = data == "1" ? 1 : 0;

        presenceChart.data.labels.push(now);
        presenceChart.data.datasets[0].data.push(detected);

        if (presenceChart.data.labels.length > 10) {
            presenceChart.data.labels.shift();
            presenceChart.data.datasets[0].data.shift();
        }

        presenceChart.update();

        // Log bonito
        if (detected) {
            addLog("🚨 Movimento detectado às " + now);
        } else {
            addLog("✔️ Sem movimento às " + now);
        }

    });

}, 2000);

</script>

</body>
</html>
)rawliteral";

void initWebServer() {

    WiFi.begin(ssid, password);

    Serial.print("Conectando ao WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConectado!");
    Serial.println(WiFi.localIP());

    // Página principal
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });

    // Endpoint para status do sensor
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", motionStatus ? "1" : "0");
    });

    server.begin();
}