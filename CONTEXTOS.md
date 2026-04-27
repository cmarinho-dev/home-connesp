Você é um engenheiro de software especialista em sistemas embarcados, IoT e ESP32.

Crie um sistema completo chamado **HomeConnesp**, baseado nas especificações abaixo.

---

# 📌 CONTEXTO DO PROJETO

Sistema ciberfísico de automação residencial com ESP32, integrando:

* Sensores
* Atuadores
* Interface web embarcada

Com foco em:

* Monitoramento em tempo real
* Performance
* Controle inteligente de ambiente

---

# 🎨 FRONTEND (OBRIGATÓRIO)

A interface web deve seguir estas tecnologias:

## 🎯 Estilização

* Usar **BeerCSS**
* Interface moderna, responsiva e simples
* Usar:

  * Cards
  * Botões estilizados
  * Layout organizado (dashboard)

---

## 📊 Gráficos

* Usar **Chart.js**

* Implementar gráficos para:

  * Uso de CPU
  * Uso de memória
  * Dados dos sensores (tempo real)
  * Histórico (simulado ou persistido)

* Gráficos devem ser:

  * Atualizados dinamicamente (AJAX ou fetch)
  * Simples e leves

---

# ⚙️ FUNCIONALIDADES (RESUMO)

## Sensores

* PIR → movimento
* Reed Switch → porta

## Atuadores

* Relé → luz/ventilador
* Servo → tranca da porta

---

## Lógica inteligente

* Entrada → liga dispositivos
* Saída → desliga dispositivos
* Registrar logs de eventos

---

## Interface Web deve ter:

### 📊 Dashboard

* Status geral
* Gráficos (Chart.js)
* Dados em tempo real

### 🎛️ Controle

* Botões:

  * Ligar/desligar relé
  * Trancar/destrancar porta

### 📜 Logs

* Lista de eventos (INFO/WARN/ERROR)

### ⚙️ Configurações

* Frequência dos sensores
* Wi-Fi

---

# 🧵 BACKEND (ESP32)

* C++ (Arduino framework)
* FreeRTOS (multitarefa)
* Sem delay()
* Usar interrupções
* Servidor web embutido

---

# 💾 Persistência

* Logs por 24h (SPIFFS ou LittleFS)

---

# 📦 SAÍDA ESPERADA

Gerar:

1. Código completo ESP32
2. HTML com BeerCSS
3. JS com Chart.js
4. Integração frontend + backend
5. Estrutura do projeto

---

# ⚠️ IMPORTANTE

* Código funcional
* Interface bonita e organizada
* Gráficos funcionando (mesmo que com dados simulados inicialmente)
* Fácil de evoluir

---

Gere tudo passo a passo.
