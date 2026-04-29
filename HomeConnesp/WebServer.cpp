#include "WebServer.h"
#include "Config.h"
#include "Logger.h"
#include "Sensors.h"
#include "Actuators.h"
#include "Performance.h"
#include <WiFi.h>

static AsyncWebServer server(80);

// ─── HTML ────────────────────────────────────────────────────────────────────
static const char HTML[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>HomeConnesp</title>
<link href="https://cdn.jsdelivr.net/npm/beercss@3.7.14/dist/cdn/beer.min.css" rel="stylesheet">
<link href="https://fonts.googleapis.com/icon?family=Material+Icons" rel="stylesheet">
<script type="module" src="https://cdn.jsdelivr.net/npm/beercss@3.7.14/dist/cdn/beer.min.js"></script>
<script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.3/dist/chart.umd.min.js"></script>
<style>
  body{background:#0f172a;color:#e2e8f0}
  .page{display:none}.page.active{display:block}
  .nav-item{cursor:pointer;border-radius:12px;padding:8px 16px;margin:2px 0;display:flex;align-items:center;gap:12px}
  .nav-item:hover{background:rgba(99,102,241,.2)}
  .nav-item.active{background:rgba(99,102,241,.4);color:#818cf8}
  .card{background:#1e293b;border-radius:16px;padding:20px;margin-bottom:16px}
  .chip-info{background:#1d4ed8;color:#fff;border-radius:20px;padding:2px 10px;font-size:.75rem}
  .chip-warn{background:#b45309;color:#fff;border-radius:20px;padding:2px 10px;font-size:.75rem}
  .chip-error{background:#b91c1c;color:#fff;border-radius:20px;padding:2px 10px;font-size:.75rem}
  .metric-val{font-size:2rem;font-weight:700;color:#818cf8}
  .metric-lbl{font-size:.8rem;color:#94a3b8;margin-top:4px}
  .log-row{display:flex;gap:10px;align-items:flex-start;padding:8px;border-radius:8px;background:#0f172a;margin-bottom:6px}
  .status-dot{width:10px;height:10px;border-radius:50%;display:inline-block;margin-right:4px}
  .dot-on{background:#22c55e}.dot-off{background:#ef4444}
  input[type=range]{width:100%;accent-color:#818cf8}
  table{width:100%;border-collapse:collapse;font-size:.85rem}
  th{background:#0f172a;padding:8px;text-align:left;color:#94a3b8}
  td{padding:8px;border-bottom:1px solid #334155}
  .btn-primary{background:#4f46e5;color:#fff;border:none;border-radius:10px;padding:10px 20px;cursor:pointer;font-size:.9rem}
  .btn-primary:hover{background:#6366f1}
  .btn-danger{background:#dc2626;color:#fff;border:none;border-radius:10px;padding:10px 20px;cursor:pointer}
  .btn-success{background:#16a34a;color:#fff;border:none;border-radius:10px;padding:10px 20px;cursor:pointer}
  .sidebar{flex-direction:column;align-items:start;justify-content:center;width:220px;min-height:100vh;background:#1e293b;padding:16px;position:fixed;top:0;left:0}
  .main-content{margin-left:236px;padding:24px}
  @media(max-width:768px){.sidebar{display:none}.main-content{margin-left:0}}
  .grid-2{display:grid;grid-template-columns:1fr 1fr;gap:16px}
  .grid-4{display:grid;grid-template-columns:repeat(4,1fr);gap:12px}
  @media(max-width:900px){.grid-4{grid-template-columns:repeat(2,1fr)}.grid-2{grid-template-columns:1fr}}
  .toggle-label{display:flex;align-items:center;gap:10px;cursor:pointer}
  .switch{position:relative;display:inline-block;width:44px;height:24px;vertical-align:middle}
  .switch input{position:absolute;inset:0;opacity:0;margin:0;padding:0;z-index:3;cursor:pointer;-webkit-appearance:none}
  .slider-sw{position:absolute;top:0;left:0;right:0;bottom:0;pointer-events:none;box-sizing:border-box;background:#334155;border-radius:24px;transition:background .2s}
  .slider-sw:before{position:absolute;content:"";height:18px;width:18px;left:3px;top:3px;background:#fff;border-radius:50%;transition:transform .2s;box-shadow:0 1px 2px rgba(0,0,0,0.15);transform:translateX(0)}
  .switch input:checked + .slider-sw{background:#4f46e5}
  .switch input:checked + .slider-sw:before{transform:translateX(20px)}
  fieldset{border:1px solid #334155;border-radius:12px;padding:16px;margin-bottom:12px}
  legend{color:#818cf8;padding:0 8px;font-size:.85rem}
  .field{margin-bottom:12px}
  .field label{display:block;font-size:.8rem;color:#94a3b8;margin-bottom:4px}
  .field input,.field select{width:100%;background:#0f172a;border:1px solid #334155;border-radius:8px;padding:8px;color:#e2e8f0;box-sizing:border-box}
  .field input:focus{outline:none;border-color:#4f46e5}
  .about-card{text-align:center;padding:32px}
  .changelog-entry{border-left:3px solid #4f46e5;padding:8px 16px;margin-bottom:12px;background:#0f172a;border-radius:0 8px 8px 0}
  .version-tag{background:#4f46e5;color:#fff;border-radius:6px;padding:2px 8px;font-size:.75rem;margin-right:8px}
  canvas{max-height:220px}
</style>
</head>
<body>

<!-- SIDEBAR -->
<nav class="sidebar">
  <div style="font-size:1.2rem;font-weight:700;color:#818cf8;margin-bottom:24px;display:flex;align-items:center;gap:8px">
    <span class="material-icons">home</span> HomeConnesp
  </div>
  <div class="nav-item active" onclick="showPage('dashboard',this)"><span class="material-icons">dashboard</span>Dashboard</div>
  <div class="nav-item" onclick="showPage('performance',this)"><span class="material-icons">speed</span>Performance</div>
  <div class="nav-item" onclick="showPage('logs',this)"><span class="material-icons">article</span>Logs</div>
  <div class="nav-item" onclick="showPage('control',this)"><span class="material-icons">toggle_on</span>Controle</div>
  <div class="nav-item" onclick="showPage('config',this)"><span class="material-icons">settings</span>Config</div>
  <div class="nav-item" onclick="showPage('about',this)"><span class="material-icons">info</span>Sobre</div>
  <div class="nav-item" onclick="showPage('changelog',this)"><span class="material-icons">history</span>Changelog</div>
  <div style="position:absolute;bottom:16px;left:16px;right:16px;font-size:.75rem;color:#475569">
    IP: <span id="wifiIP">--</span><br>
    RSSI: <span id="wifiRSSI">--</span> dBm<br>
    Uptime: <span id="uptime">--</span>
  </div>
</nav>

<main class="main-content">

<!-- ══════════ DASHBOARD ══════════ -->
<div id="page-dashboard" class="page active">
  <h2 style="margin-bottom:20px">Dashboard</h2>
  <div class="grid-4">
    <div class="card" style="text-align:center">
      <div class="material-icons" style="font-size:2rem;color:#f59e0b;padding-block:18px">sensors</div>
      <div class="metric-val" id="d-motion">--</div>
      <div class="metric-lbl">PIR / Movimento</div>
    </div>
    <div class="card" style="text-align:center">
      <div class="material-icons" style="font-size:2rem;color:#3b82f6;padding-block:18px">door_front</div>
      <div class="metric-val" id="d-door">--</div>
      <div class="metric-lbl">Porta (Reed)</div>
    </div>
    <div class="card" style="text-align:center">
      <div class="material-icons" style="font-size:2rem;color:#22c55e;padding-block:18px">power</div>
      <div class="metric-val" id="d-relay">--</div>
      <div class="metric-lbl">Relé</div>
    </div>
    <div class="card" style="text-align:center">
      <div class="material-icons" style="font-size:2rem;color:#a855f7;padding-block:18px">lock</div>
      <div class="metric-val" id="d-servo">--</div>
      <div class="metric-lbl">Servo (graus)</div>
    </div>
  </div>
  <div class="grid-4" style="margin-top:4px">
    <div class="card" style="text-align:center">
      <div class="metric-val" id="d-cpu">--%</div>
      <div class="metric-lbl">CPU Load</div>
    </div>
    <div class="card" style="text-align:center">
      <div class="metric-val" id="d-heap">-- KB</div>
      <div class="metric-lbl">Heap Livre</div>
    </div>
    <div class="card" style="text-align:center">
      <div class="metric-val" id="d-rssi">--</div>
      <div class="metric-lbl">Wi-Fi RSSI (dBm)</div>
    </div>
    <div class="card" style="text-align:center">
      <div class="metric-val" id="d-uptime">--</div>
      <div class="metric-lbl">Uptime (s)</div>
    </div>
  </div>
  <div class="grid-2">
    <div class="card"><canvas id="chartCpu"></canvas></div>
    <div class="card"><canvas id="chartHeap"></canvas></div>
  </div>
  <div class="grid-2">
    <div class="card"><canvas id="chartPir"></canvas></div>
    <div class="card"><canvas id="chartDoor"></canvas></div>
  </div>
</div>

<!-- ══════════ PERFORMANCE ══════════ -->
<div id="page-performance" class="page">
  <h2 style="margin-bottom:20px">Performance</h2>
  <div class="grid-4">
    <div class="card" style="text-align:center">
      <div class="metric-val" id="p-cpu">--%</div><div class="metric-lbl">CPU Load</div>
    </div>
    <div class="card" style="text-align:center">
      <div class="metric-val" id="p-heap-free">--</div><div class="metric-lbl">Heap Livre (B)</div>
    </div>
    <div class="card" style="text-align:center">
      <div class="metric-val" id="p-heap-min">--</div><div class="metric-lbl">Heap Mín. (B)</div>
    </div>
    <div class="card" style="text-align:center">
      <div class="metric-val" id="p-flash">--</div><div class="metric-lbl">Flash Usada (KB)</div>
    </div>
  </div>
  <div class="grid-2">
    <div class="card">
      <b>Memória</b><br><br>
      <canvas id="chartMem"></canvas>
    </div>
    <div class="card">
      <b>PSRAM / Flash</b><br><br>
      <canvas id="chartFlash"></canvas>
    </div>
  </div>
  <div class="card">
    <b>Tempo de Execução das Funções (µs)</b>
    <div style="margin-top:12px"><canvas id="chartFuncs"></canvas></div>
  </div>
  <div class="card">
    <b>Detalhes das Funções</b>
    <table style="margin-top:12px">
      <thead><tr><th>Função</th><th>Último</th><th>Mín</th><th>Máx</th><th>Média</th><th>Chamadas</th></tr></thead>
      <tbody id="func-table"></tbody>
    </table>
  </div>
  <div class="card">
    <b>Tasks FreeRTOS</b>
    <table style="margin-top:12px">
      <thead><tr><th>Nome</th><th>Prioridade</th><th>Stack Livre (B)</th><th>Estado</th></tr></thead>
      <tbody id="task-table"></tbody>
    </table>
  </div>
</div>

<!-- ══════════ LOGS ══════════ -->
<div id="page-logs" class="page">
  <h2 style="margin-bottom:16px">Logs do Sistema</h2>
  <div style="display:flex;gap:10px;flex-wrap:wrap;margin-bottom:16px">
    <button class="btn-primary" onclick="filterLogs('ALL')">Todos</button>
    <button class="btn-primary" onclick="filterLogs('INFO')" style="background:#1d4ed8">INFO</button>
    <button class="btn-primary" onclick="filterLogs('WARN')" style="background:#b45309">WARN</button>
    <button class="btn-primary" onclick="filterLogs('ERROR')" style="background:#b91c1c">ERROR</button>
    <button class="btn-primary" onclick="exportLogs()" style="background:#0f766e">Exportar CSV</button>
    <button class="btn-danger" onclick="clearLogs()">Limpar Logs</button>
  </div>
  <div class="card">
    <div id="log-container" style="max-height:60vh;overflow-y:auto"></div>
  </div>
</div>

<!-- ══════════ CONTROLE ══════════ -->
<div id="page-control" class="page">
  <h2 style="margin-bottom:20px">Controle Manual</h2>
  <div class="grid-2">
    <div class="card">
      <b style="font-size:1.1rem">Relé (Luz / Ventilador)</b>
      <div style="margin:20px 0;font-size:.9rem;color:#94a3b8">Estado atual: <span id="c-relay-state" style="color:#e2e8f0;font-weight:600">--</span></div>
      <div style="display:flex;gap:12px">
        <button class="btn-success" onclick="ctrlRelay(true)">Ligar</button>
        <button class="btn-danger"  onclick="ctrlRelay(false)">Desligar</button>
      </div>
    </div>
    <div class="card">
      <b style="font-size:1.1rem">Servo (Tranca)</b>
      <div style="margin:16px 0">
        <label style="font-size:.85rem;color:#94a3b8">Ângulo: <span id="servo-val">0</span>°</label>
        <input type="range" min="0" max="180" value="0" id="servo-range" oninput="document.getElementById('servo-val').textContent=this.value">
      </div>
      <div style="display:flex;gap:12px;flex-wrap:wrap">
        <button class="btn-primary" onclick="ctrlServo(document.getElementById('servo-range').value)">Aplicar</button>
        <button class="btn-success" onclick="ctrlServo(180);document.getElementById('servo-range').value=180;document.getElementById('servo-val').textContent=180">Abrir (180°)</button>
        <button class="btn-danger"  onclick="ctrlServo(0);document.getElementById('servo-range').value=0;document.getElementById('servo-val').textContent=0">Fechar (0°)</button>
      </div>
    </div>
  </div>
  <div class="card">
    <b>Modo Automático</b>
    <div style="margin-top:12px;color:#94a3b8;font-size:.9rem">Quando ativo, o sistema controla relé e servo automaticamente via sensores.</div>
    <div style="margin-top:16px">
      <label class="toggle-label">
        <span class="switch"><input type="checkbox" id="auto-mode" onchange="ctrlAutoMode(this.checked)"><span class="slider-sw"></span></span>
        <span id="auto-mode-lbl">Desativado</span>
      </label>
    </div>
  </div>
</div>

<!-- ══════════ CONFIG ══════════ -->
<div id="page-config" class="page">
  <h2 style="margin-bottom:20px">Configurações</h2>
  <div class="card">
    <fieldset>
      <legend>Sensores</legend>
      <div class="field">
        <label>Intervalo de leitura (ms): <span id="cfg-interval-val">2000</span></label>
        <input type="range" min="500" max="10000" step="500" value="2000" id="cfg-interval"
               oninput="document.getElementById('cfg-interval-val').textContent=this.value">
      </div>
    </fieldset>
    <fieldset>
      <legend>Energia</legend>
      <label class="toggle-label">
        <span class="switch"><input type="checkbox" id="cfg-sleep"><span class="slider-sw"></span></span>
        <span>Habilitar Light Sleep (quando inativo &gt;5min)</span>
      </label>
    </fieldset>
    <fieldset>
      <legend>Wi-Fi</legend>
      <div class="field">
        <label>SSID</label>
        <input type="text" id="cfg-ssid" placeholder="Nome da rede">
      </div>
      <div class="field">
        <label>Senha</label>
        <input type="password" id="cfg-pass" placeholder="Senha do Wi-Fi">
      </div>
      <div style="font-size:.8rem;color:#f59e0b;margin-top:4px">⚠ Alterar Wi-Fi causa reconexão do dispositivo.</div>
    </fieldset>
    <button class="btn-primary" onclick="saveConfig()">Salvar Configurações</button>
    <span id="cfg-msg" style="margin-left:12px;font-size:.85rem;color:#22c55e"></span>
  </div>
</div>

<!-- ══════════ SOBRE ══════════ -->
<div id="page-about" class="page">
  <div class="card about-card">
    <div class="material-icons" style="font-size:4rem;color:#818cf8">home</div>
    <h2 style="color:#818cf8;margin:8px 0">HomeConnesp</h2>
    <p style="color:#94a3b8">Sistema Ciberfísico de Automação Residencial com ESP32</p>
    <hr style="border-color:#334155;margin:20px 0">
    <div style="font-size:1rem;color:#94a3b8">
      <b style="color:#e2e8f0">Universidade</b><br>
      Universidade Estadual de Londrina — UEL<br>
      Departamento de Computação
    </div>
    <hr style="border-color:#334155;margin:20px 0">
    <b style="color:#e2e8f0">Integrantes</b>
    <ul style="color:#94a3b8;margin-top:8px;font-size:.9rem">
      <li>Carlos Henrique</li>
      <li>Alan Mendes</li>
      <li>José Ricardo</li>
      <li>Rafael Berton</li>
      <li>Rafael Padilha</li>
    </ul>
  </div>
</div>

<!-- ══════════ CHANGELOG ══════════ -->
<div id="page-changelog" class="page">
  <h2 style="margin-bottom:20px">Changelog</h2>
  <div class="changelog-entry">
    <div><span class="version-tag">v1.0.0</span><b>Lançamento inicial</b></div>
    <ul style="color:#94a3b8;margin-top:8px;font-size:.9rem">
      <li>Dashboard com gráficos em tempo real (CPU, heap, PIR, porta)</li>
      <li>Monitoramento de performance: heap, flash, PSRAM, funções, tasks FreeRTOS</li>
      <li>Sistema de logs (INFO/WARN/ERROR) com persistência LittleFS 24h</li>
      <li>Controle manual do relé e servo via interface web</li>
      <li>Modo automático: relé ativado por PIR, servo por reed switch</li>
      <li>Interrupções de hardware nos sensores PIR e Reed Switch</li>
      <li>Multitarefa FreeRTOS: Sensors, Actuators, Performance, Logger, WebMonitor</li>
      <li>Estimativa de CPU load via idle counter task</li>
      <li>Light Sleep configurável via interface</li>
      <li>Configuração de Wi-Fi e intervalo de sensores sem regravar firmware</li>
      <li>Persistência de config via Preferences (NVS)</li>
      <li>Interface com BeerCSS + Chart.js, tema escuro responsivo</li>
    </ul>
  </div>
  <div class="changelog-entry" style="border-color:#334155">
    <div><span class="version-tag" style="background:#334155">v0.9.0</span><b>Beta</b></div>
    <ul style="color:#94a3b8;margin-top:8px;font-size:.9rem">
      <li>Protótipo inicial com PIR, dashboard básico e Tailwind CSS</li>
      <li>Webserver assíncrono (ESPAsyncWebServer)</li>
    </ul>
  </div>
</div>

</main>

<script>
// ── Navigation ──────────────────────────────────────────────────────────────
function showPage(id, el) {
  document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.nav-item').forEach(n => n.classList.remove('active'));
  document.getElementById('page-' + id).classList.add('active');
  if (el) el.classList.add('active');
  if (id === 'performance') updatePerformance();
  if (id === 'logs')        updateLogs();
  if (id === 'config')      loadConfig();
  if (id === 'control')     updateControl();
}

// ── Charts ───────────────────────────────────────────────────────────────────
const MAX_PTS = 20;
function mkLineChart(id, label, color) {
  const ctx = document.getElementById(id).getContext('2d');
  return new Chart(ctx, {
    type: 'line',
    data: { labels: [], datasets: [{ label, data: [], borderColor: color, backgroundColor: color+'22', tension: 0.4, pointRadius: 2, fill: true }] },
    options: { animation: false, plugins: { legend: { labels: { color: '#94a3b8' } } }, scales: {
      x: { ticks: { color: '#64748b', maxTicksLimit: 6 }, grid: { color: '#1e293b' } },
      y: { ticks: { color: '#64748b' }, grid: { color: '#334155' } }
    }}
  });
}
function pushPoint(chart, label, value) {
  chart.data.labels.push(label);
  chart.data.datasets[0].data.push(value);
  if (chart.data.labels.length > MAX_PTS) { chart.data.labels.shift(); chart.data.datasets[0].data.shift(); }
  chart.update('none');
}

const chartCpu  = mkLineChart('chartCpu',  'CPU (%)',      '#818cf8');
const chartHeap = mkLineChart('chartHeap', 'Heap livre (KB)', '#22c55e');
const chartPir  = mkLineChart('chartPir',  'PIR (movimento)', '#f59e0b');
const chartDoor = mkLineChart('chartDoor', 'Porta (aberta)',  '#3b82f6');

// Performance charts (created lazily)
let chartMem = null, chartFlash = null, chartFuncs = null;

function mkBarChart(id, labels, datasets) {
  const ctx = document.getElementById(id).getContext('2d');
  return new Chart(ctx, {
    type: 'bar',
    data: { labels, datasets },
    options: { animation: false, indexAxis: 'y', plugins: { legend: { labels: { color: '#94a3b8' } } }, scales: {
      x: { ticks: { color: '#64748b' }, grid: { color: '#334155' } },
      y: { ticks: { color: '#94a3b8' }, grid: { color: '#1e293b' } }
    }}
  });
}

// ── API helpers ──────────────────────────────────────────────────────────────
const api = path => fetch(path).then(r => r.json()).catch(() => null);
const post = (path, body) => fetch(path, { method: 'POST', headers: {'Content-Type':'application/json'}, body: JSON.stringify(body) });

function ts() { return new Date().toLocaleTimeString('pt-BR'); }
function kb(b) { return (b/1024).toFixed(0); }

// ── Dashboard polling ────────────────────────────────────────────────────────
let logFilter = 'ALL';

async function pollDashboard() {
  const d = await api('/api/status');
  if (!d) return;
  const t = ts();

  document.getElementById('d-motion').textContent  = d.motion ? 'SIM' : 'NÃO';
  document.getElementById('d-door').textContent    = d.door   ? 'ABERTA' : 'FECHADA';
  document.getElementById('d-relay').textContent   = d.relay  ? 'LIGADO' : 'DESLIGADO';
  document.getElementById('d-servo').textContent   = d.servo + '°';
  document.getElementById('d-cpu').textContent     = d.cpu.toFixed(1) + '%';
  document.getElementById('d-heap').textContent    = kb(d.heap_free) + ' KB';
  document.getElementById('d-rssi').textContent    = d.rssi;
  document.getElementById('d-uptime').textContent  = d.uptime + 's';
  document.getElementById('wifiRSSI').textContent  = d.rssi;
  document.getElementById('wifiIP').textContent    = d.ip || '--';
  document.getElementById('uptime').textContent    = d.uptime + 's';

  pushPoint(chartCpu,  t, d.cpu);
  pushPoint(chartHeap, t, d.heap_free / 1024);
  pushPoint(chartPir,  t, d.motion ? 1 : 0);
  pushPoint(chartDoor, t, d.door   ? 1 : 0);
}
setInterval(pollDashboard, 2000);
pollDashboard();

// ── Performance page ─────────────────────────────────────────────────────────
async function updatePerformance() {
  const d = await api('/api/performance');
  if (!d) return;

  document.getElementById('p-cpu').textContent       = d.cpu.toFixed(1) + '%';
  document.getElementById('p-heap-free').textContent = d.heap_free;
  document.getElementById('p-heap-min').textContent  = d.heap_min;
  document.getElementById('p-flash').textContent     = kb(d.sketch_size) + ' KB';

  // Memory bar chart
  const memLabels = ['Heap Livre','Heap Usada','Heap Mín. Livre'];
  const memData   = [d.heap_free, d.heap_total - d.heap_free, d.heap_min];
  if (!chartMem) {
    chartMem = mkBarChart('chartMem', memLabels, [{ label:'Bytes', data: memData, backgroundColor:['#22c55e','#ef4444','#f59e0b'] }]);
  } else { chartMem.data.datasets[0].data = memData; chartMem.update('none'); }

  // Flash donut
  const flashLabels = d.psram_total > 0
    ? ['Sketch','Flash Livre','PSRAM Livre']
    : ['Sketch','Flash Livre'];
  const flashData = d.psram_total > 0
    ? [d.sketch_size, d.flash_size - d.sketch_size, d.psram_free]
    : [d.sketch_size, d.flash_size - d.sketch_size];
  if (!chartFlash) {
    const ctx2 = document.getElementById('chartFlash').getContext('2d');
    chartFlash = new Chart(ctx2, { type: 'doughnut', data: { labels: flashLabels,
      datasets: [{ data: flashData, backgroundColor:['#818cf8','#334155','#22c55e'] }] },
      options: { animation: false, plugins: { legend: { labels: { color:'#94a3b8' } } } }
    });
  } else { chartFlash.data.datasets[0].data = flashData; chartFlash.update('none'); }

  // Functions table + chart
  const tbody = document.getElementById('func-table');
  tbody.innerHTML = '';
  const fNames = [], fAvg = [];
  (d.funcs || []).forEach(f => {
    tbody.innerHTML += `<tr><td>${f.name}</td><td>${f.last}µs</td><td>${f.min}µs</td><td>${f.max}µs</td><td>${f.avg}µs</td><td>${f.calls}</td></tr>`;
    fNames.push(f.name); fAvg.push(f.avg);
  });
  if (!chartFuncs) {
    chartFuncs = mkBarChart('chartFuncs', fNames, [{ label:'Tempo médio (µs)', data: fAvg, backgroundColor:'#818cf855' }]);
  } else { chartFuncs.data.labels = fNames; chartFuncs.data.datasets[0].data = fAvg; chartFuncs.update('none'); }

  // Tasks table
  const ttbody = document.getElementById('task-table');
  ttbody.innerHTML = '';
  const stateMap = { R:'🟢 Running', B:'🟡 Blocked', S:'⚪ Suspended', D:'🔴 Deleted' };
  (d.tasks || []).forEach(t => {
    ttbody.innerHTML += `<tr><td>${t.name}</td><td>${t.prio}</td><td>${t.stack}B</td><td>${stateMap[t.state]||t.state}</td></tr>`;
  });
}

// ── Logs page ────────────────────────────────────────────────────────────────
async function updateLogs() {
  const data = await api('/api/logs');
  if (!data) return;
  renderLogs(data);
}

function renderLogs(data) {
  const container = document.getElementById('log-container');
  container.innerHTML = '';
  const filtered = logFilter === 'ALL' ? data : data.filter(e => e.level === logFilter);
  [...filtered].reverse().forEach(e => {
    const chipClass = e.level === 'ERROR' ? 'chip-error' : e.level === 'WARN' ? 'chip-warn' : 'chip-info';
    const uptime = (e.ts/1000).toFixed(1) + 's';
    container.innerHTML += `<div class="log-row"><span class="${chipClass}">${e.level}</span><span style="color:#64748b;font-size:.8rem;white-space:nowrap">${uptime}</span><span style="flex:1">${e.msg}</span></div>`;
  });
}

function filterLogs(f) { logFilter = f; updateLogs(); }

function exportLogs() { window.open('/api/logs/export', '_blank'); }

async function clearLogs() {
  if (!confirm('Limpar todos os logs?')) return;
  await fetch('/api/logs/clear', { method: 'POST' });
  updateLogs();
}

// ── Control page ─────────────────────────────────────────────────────────────
async function updateControl() {
  const d = await api('/api/status');
  if (!d) return;
  document.getElementById('c-relay-state').textContent = d.relay ? 'LIGADO' : 'DESLIGADO';
  document.getElementById('servo-range').value = d.servo;
  document.getElementById('servo-val').textContent = d.servo;
  const am = document.getElementById('auto-mode');
  am.checked = d.auto_mode;
  document.getElementById('auto-mode-lbl').textContent = d.auto_mode ? 'Ativado' : 'Desativado';
}

async function ctrlRelay(on) {
  await post('/api/relay', { state: on });
  document.getElementById('c-relay-state').textContent = on ? 'LIGADO' : 'DESLIGADO';
}
async function ctrlServo(angle) {
  await post('/api/servo', { angle: parseInt(angle) });
}
async function ctrlAutoMode(en) {
  await post('/api/automode', { enabled: en });
  document.getElementById('auto-mode-lbl').textContent = en ? 'Ativado' : 'Desativado';
}

// ── Config page ───────────────────────────────────────────────────────────────
async function loadConfig() {
  const d = await api('/api/config');
  if (!d) return;
  document.getElementById('cfg-interval').value = d.sensor_interval;
  document.getElementById('cfg-interval-val').textContent = d.sensor_interval;
  document.getElementById('cfg-sleep').checked = d.light_sleep;
  document.getElementById('cfg-ssid').value = d.ssid || '';
}
async function saveConfig() {
  const body = {
    sensor_interval: parseInt(document.getElementById('cfg-interval').value),
    light_sleep:     document.getElementById('cfg-sleep').checked,
    ssid:            document.getElementById('cfg-ssid').value,
    password:        document.getElementById('cfg-pass').value
  };
  const r = await post('/api/config', body);
  const msg = document.getElementById('cfg-msg');
  msg.textContent = r.ok ? '✔ Salvo!' : '✘ Erro';
  setTimeout(() => msg.textContent = '', 3000);
}

// Auto-refresh performance when on that page
setInterval(() => {
  if (document.getElementById('page-performance').classList.contains('active')) updatePerformance();
  if (document.getElementById('page-logs').classList.contains('active'))        updateLogs();
  if (document.getElementById('page-control').classList.contains('active'))     updateControl();
}, 5000);
</script>
</body>
</html>
)HTML";

// ─── Route helpers ────────────────────────────────────────────────────────────
static String statusJSON() {
    String j = "{";
    j += "\"motion\":"    + String(Sensors::getMotion() ? "true" : "false") + ",";
    j += "\"door\":"      + String(Sensors::getDoor()   ? "true" : "false") + ",";
    j += "\"relay\":"     + String(Actuators::getRelay()? "true" : "false") + ",";
    j += "\"servo\":"     + String(Actuators::getServo())                   + ",";
    j += "\"auto_mode\":" + String(Actuators::getAutoMode()? "true":"false")+ ",";
    j += "\"cpu\":"       + String(Performance::getCpuLoad(), 1)            + ",";
    j += "\"heap_free\":" + String(Performance::getHeapFree())              + ",";
    j += "\"rssi\":"      + String(WiFi.RSSI())                             + ",";
    j += "\"ip\":\""      + WiFi.localIP().toString()                       + "\",";
    j += "\"uptime\":"    + String(Performance::getUptime());
    j += "}";
    return j;
}

static String bodyString(AsyncWebServerRequest* request) {
    // Body already consumed by body handler; used via lambda capture below
    return "";
}

// ─── Init ─────────────────────────────────────────────────────────────────────
void initWebServer() {
    // ── Connect Wi-Fi ──────────────────────────────────────────────────────────
    WiFi.begin(Config::getSSID().c_str(), Config::getPassword().c_str());
    Serial.print("Conectando ao WiFi");
    uint8_t tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries++ < 20) {
        vTaskDelay(pdMS_TO_TICKS(500));
        Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConectado: " + WiFi.localIP().toString());
        Logger::log(LOG_INFO, ("WiFi OK IP=" + WiFi.localIP().toString()).c_str());
    } else {
        Serial.println("\nWiFi falhou, continuando sem rede...");
        Logger::log(LOG_WARN, "WiFi nao conectado");
    }

    // ── Static page ────────────────────────────────────────────────────────────
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send_P(200, "text/html", HTML);
    });

    // ── GET /api/status ────────────────────────────────────────────────────────
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", statusJSON());
    });

    // ── GET /api/performance ───────────────────────────────────────────────────
    server.on("/api/performance", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", Performance::toJSON());
    });

    // ── GET /api/logs ──────────────────────────────────────────────────────────
    server.on("/api/logs", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", Logger::toJSON());
    });

    // ── GET /api/logs/export ───────────────────────────────────────────────────
    server.on("/api/logs/export", HTTP_GET, [](AsyncWebServerRequest* req) {
        AsyncWebServerResponse* resp = req->beginResponse(200, "text/csv", Logger::exportCSV());
        resp->addHeader("Content-Disposition", "attachment; filename=homeconnesp_logs.csv");
        req->send(resp);
    });

    // ── POST /api/logs/clear ───────────────────────────────────────────────────
    server.on("/api/logs/clear", HTTP_POST, [](AsyncWebServerRequest* req) {
        Logger::clear();
        Logger::log(LOG_INFO, "Logs limpos via interface");
        req->send(200, "application/json", "{\"ok\":true}");
    });

    // ── GET /api/config ────────────────────────────────────────────────────────
    server.on("/api/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", Config::toJSON());
    });

    // ── POST /api/config ───────────────────────────────────────────────────────
    server.on("/api/config", HTTP_POST,
        [](AsyncWebServerRequest* req) {
            req->send(200, "application/json", "{\"ok\":true}");
        },
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            String body = String((char*)data, len);
            Config::fromJSON(body);
            Config::save();
            Logger::log(LOG_INFO, "Config salva via web");
        }
    );

    // ── POST /api/relay ────────────────────────────────────────────────────────
    server.on("/api/relay", HTTP_POST,
        [](AsyncWebServerRequest* req) {
            req->send(200, "application/json", "{\"ok\":true}");
        },
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            String body = String((char*)data, len);
            bool on = body.indexOf("true") >= 0;
            Actuators::setRelay(on);
            Logger::log(LOG_INFO, on ? "Rele ligado via web" : "Rele desligado via web");
        }
    );

    // ── POST /api/servo ────────────────────────────────────────────────────────
    server.on("/api/servo", HTTP_POST,
        [](AsyncWebServerRequest* req) {
            req->send(200, "application/json", "{\"ok\":true}");
        },
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            String body = String((char*)data, len);
            int idx = body.indexOf("\"angle\":");
            if (idx >= 0) {
                int a = body.substring(idx + 8).toInt();
                Actuators::setServo(a);
                Logger::log(LOG_INFO, ("Servo=" + String(a) + " graus via web").c_str());
            }
        }
    );

    // ── POST /api/automode ─────────────────────────────────────────────────────
    server.on("/api/automode", HTTP_POST,
        [](AsyncWebServerRequest* req) {
            req->send(200, "application/json", "{\"ok\":true}");
        },
        nullptr,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
            String body = String((char*)data, len);
            bool en = body.indexOf("true") >= 0;
            Actuators::setAutoMode(en);
            Logger::log(LOG_INFO, en ? "Modo auto ativado" : "Modo auto desativado");
        }
    );

    server.begin();
    Logger::log(LOG_INFO, "WebServer iniciado na porta 80");
}
