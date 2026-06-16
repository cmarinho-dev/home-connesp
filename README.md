# HomeConnesp

<p align="center">
  <strong>Sistema ciberfísico de automação residencial com ESP32</strong>
</p>

<p align="center">
  <img alt="ESP32" src="https://img.shields.io/badge/ESP32-IoT-blue">
  <img alt="Arduino" src="https://img.shields.io/badge/Arduino-Framework-00979D">
  <img alt="FreeRTOS" src="https://img.shields.io/badge/FreeRTOS-Multitarefa-green">
  <img alt="C++" src="https://img.shields.io/badge/C%2B%2B-Embedded-00599C">
  <img alt="Status" src="https://img.shields.io/badge/status-em%20desenvolvimento-yellow">
</p>

---

## 📌 Sobre o projeto

**HomeConnesp** é um sistema ciberfísico de automação residencial desenvolvido com **ESP32**, integrando sensores, atuadores, monitoramento de desempenho e uma interface web embarcada.

O projeto foi desenvolvido com foco em:

- Monitoramento em tempo real;
- Controle inteligente de ambiente;
- Baixo consumo de energia;
- Registro de eventos;
- Visualização de métricas de performance;
- Interface web simples, responsiva e acessível.

A solução permite acompanhar o estado de sensores, controlar dispositivos físicos e visualizar informações do sistema diretamente pelo navegador, sem depender de um servidor externo.

---

## 🎯 Objetivo

Criar uma plataforma embarcada capaz de monitorar e controlar elementos de uma residência inteligente utilizando o ESP32 como núcleo computacional.

O sistema combina hardware, firmware e interface web para demonstrar conceitos de **IoT**, **sistemas embarcados**, **multitarefa**, **interrupções**, **persistência local** e **performance em sistemas ciberfísicos**.

---

## ⚙️ Funcionalidades

### Sensores

- Detecção de movimento com sensor **PIR**;
- Monitoramento de abertura/fechamento de porta com **Reed Switch**;
- Leitura baseada em interrupções de hardware;
- Intervalo de leitura configurável pela interface web.

### Atuadores

- Controle de **relé** para luz, ventilador ou carga externa;
- Controle de **servo motor** para simulação de tranca;
- Modo manual pela interface web;
- Modo automático baseado nos sensores.

### Monitoramento de performance

- Carga estimada da CPU;
- Heap livre, heap mínimo e memória total;
- Informações de flash e sketch;
- Tempo de execução de funções;
- Estado das tasks FreeRTOS;
- Uptime do sistema.

### Logs

- Registro de eventos do sistema;
- Níveis `INFO`, `WARN` e `ERROR`;
- Persistência local com **LittleFS**;
- Exportação dos logs em CSV;
- Limpeza de logs pela interface web;
- Retenção aproximada de 24 horas.

### Interface web

- Dashboard com dados em tempo real;
- Gráficos com **Chart.js**;
- Interface estilizada com **BeerCSS**;
- Controle manual de relé e servo;
- Configuração do intervalo dos sensores;
- Ativação/desativação do Light Sleep;
- Tela de logs e changelog.

---

## 🧠 Lógica de funcionamento

O sistema atua de forma automática quando o modo automático está habilitado:

- Quando o sensor PIR detecta movimento, o relé é acionado;
- Após um período sem movimento, o relé é desligado;
- Quando a porta é aberta, o servo é posicionado para simular destravamento;
- Quando a porta é fechada, o servo retorna para a posição de travamento;
- Eventos relevantes são registrados no sistema de logs.

Além disso, o sistema monitora o Wi-Fi e pode entrar em **Light Sleep** quando permanece ocioso, reduzindo o consumo de energia.

---

## 🧱 Arquitetura do sistema

```text
┌──────────────────────────┐
│        Interface Web      │
│  Dashboard / Controle /   │
│  Logs / Configurações     │
└─────────────┬────────────┘
              │ HTTP / API REST
┌─────────────▼────────────┐
│          ESP32            │
│ Arduino Framework +       │
│ FreeRTOS + Web Server     │
└───────┬──────────┬───────┘
        │          │
┌───────▼─────┐ ┌──▼─────────┐
│  Sensores   │ │ Atuadores  │
│ PIR / Reed  │ │ Relé/Servo │
└─────────────┘ └────────────┘
        │          │
┌───────▼──────────▼────────┐
│ Logs, Configurações e      │
│ Métricas de Performance    │
│ LittleFS + Preferences     │
└───────────────────────────┘
```

---

## 📁 Estrutura do projeto

```text
home-connesp/
├── CONTEXTOS.md
└── HomeConnesp/
    ├── HomeConnesp.ino
    ├── Config.h
    ├── Config.cpp
    ├── Sensors.h
    ├── Sensors.cpp
    ├── Actuators.h
    ├── Actuators.cpp
    ├── Performance.h
    ├── Performance.cpp
    ├── Logger.h
    ├── Logger.cpp
    ├── WebServer.h
    └── WebServer.cpp
```

### Responsabilidade dos módulos

| Módulo | Responsabilidade |
|---|---|
| `HomeConnesp.ino` | Inicialização do sistema, criação das tasks FreeRTOS e configuração das interrupções |
| `Config` | Gerenciamento de configurações persistidas em NVS/Preferences |
| `Sensors` | Leitura e armazenamento do estado dos sensores PIR e Reed Switch |
| `Actuators` | Controle do relé, servo motor e modo automático |
| `Performance` | Coleta de métricas de CPU, memória, flash, funções e tasks |
| `Logger` | Registro, persistência, exportação e limpeza de logs |
| `WebServer` | Servidor HTTP, interface web embarcada e rotas da API |

---

## 🔌 Mapeamento de pinos

| Componente | Pino ESP32 | Função |
|---|---:|---|
| Sensor PIR | GPIO 5 | Detecção de movimento |
| Reed Switch | GPIO 19 | Estado da porta |
| Relé | GPIO 21 | Controle de carga |
| Servo motor | GPIO 18 | Simulação de tranca |

> Observação: o relé foi considerado como **active-low**, ou seja, nível lógico baixo aciona o módulo.

---

## 🧰 Tecnologias utilizadas

### Hardware e firmware

- ESP32;
- Arduino Framework;
- C/C++;
- FreeRTOS;
- LittleFS;
- Preferences/NVS;
- Interrupções de hardware;
- Light Sleep.

### Interface web

- HTML;
- CSS;
- JavaScript;
- BeerCSS;
- Chart.js;
- API HTTP embarcada.

### Bibliotecas principais

- `WiFi.h`;
- `LittleFS.h`;
- `Preferences.h`;
- `ESPAsyncWebServer`;
- `AsyncTCP`;
- `ESP32Servo`.

---

## 🚀 Como executar

### 1. Clone o repositório

```bash
git clone https://github.com/cmarinho-dev/home-connesp.git
cd home-connesp
```

### 2. Abra o projeto

Abra a pasta `HomeConnesp` na **Arduino IDE** ou em outro ambiente compatível com ESP32.

O arquivo principal do projeto é:

```text
HomeConnesp/HomeConnesp.ino
```

### 3. Instale as dependências

No gerenciador de placas da Arduino IDE, instale o suporte ao **ESP32**.

Em seguida, instale as bibliotecas necessárias:

```text
ESPAsyncWebServer
AsyncTCP
ESP32Servo
```

### 4. Configure a rede Wi-Fi

Configure o SSID e a senha da rede Wi-Fi antes de compilar o firmware.

Recomendação profissional:

```cpp
// secrets.h
#define WIFI_SSID "NOME_DA_REDE"
#define WIFI_PASS "SENHA_DA_REDE"
```

E adicione `secrets.h` ao `.gitignore` para evitar o versionamento de credenciais.

### 5. Compile e envie para o ESP32

Selecione a placa ESP32 correta, escolha a porta serial e envie o firmware.

### 6. Acesse a interface web

Após iniciar, o ESP32 exibirá no monitor serial o IP obtido na rede Wi-Fi.

Acesse no navegador:

```text
http://IP_DO_ESP32
```

Exemplo:

```text
http://192.168.0.120
```

---

## 🌐 Rotas da API

| Método | Rota | Descrição |
|---|---|---|
| `GET` | `/` | Interface web principal |
| `GET` | `/api/status` | Estado geral do sistema |
| `GET` | `/api/performance` | Métricas de performance |
| `GET` | `/api/logs` | Lista de logs |
| `GET` | `/api/logs/export` | Exportação dos logs em CSV |
| `POST` | `/api/logs/clear` | Limpa os logs |
| `GET` | `/api/config` | Retorna as configurações atuais |
| `POST` | `/api/config` | Atualiza configurações |
| `POST` | `/api/relay` | Liga ou desliga o relé |
| `POST` | `/api/servo` | Ajusta o ângulo do servo |
| `POST` | `/api/automode` | Ativa ou desativa o modo automático |

### Exemplos de requisições

#### Alterar configurações

```json
{
  "sensor_interval": 2000,
  "light_sleep": true
}
```

#### Acionar relé

```json
{
  "on": true
}
```

#### Ajustar servo

```json
{
  "angle": 180
}
```

#### Ativar modo automático

```json
{
  "enabled": true
}
```

---

## 📊 Métricas acompanhadas

O sistema registra e exibe informações úteis para avaliação de performance em sistemas ciberfísicos:

- Percentual estimado de uso da CPU;
- Memória heap disponível;
- Heap mínimo registrado;
- Tamanho da flash;
- Tamanho do sketch;
- Uptime;
- Tempo de execução de funções críticas;
- Quantidade e estado das tasks FreeRTOS;
- Intensidade do sinal Wi-Fi.

Essas métricas auxiliam na análise de estabilidade, consumo de recursos e comportamento do sistema durante a execução.

---

## 🧪 Cenários de teste sugeridos

| Cenário | Resultado esperado |
|---|---|
| Movimento detectado pelo PIR | Relé acionado automaticamente |
| Sem movimento por alguns segundos | Relé desligado automaticamente |
| Porta aberta pelo Reed Switch | Servo posicionado para abertura |
| Porta fechada | Servo retorna para posição de fechamento |
| Alteração do intervalo de sensores | Novo intervalo aplicado sem regravar firmware |
| Exportação de logs | Arquivo CSV gerado pela interface web |
| Sistema ocioso | Light Sleep pode ser acionado, se habilitado |

---

## 🔐 Segurança e boas práticas

Para uso em ambiente real ou repositório público, recomenda-se:

- Não versionar SSID e senha Wi-Fi;
- Usar arquivo `secrets.h` ignorado pelo Git;
- Implementar autenticação na interface web;
- Validar melhor os dados recebidos pela API;
- Evitar exposição do ESP32 diretamente à internet;
- Adicionar proteção contra acionamentos indevidos dos atuadores;
- Documentar limites elétricos dos componentes utilizados.

---

## 🚧 Melhorias futuras

- Autenticação para acesso ao painel web;
- Portal de configuração Wi-Fi;
- Suporte a OTA Update;
- Integração com MQTT;
- Integração com Home Assistant;
- Histórico persistente de sensores;
- Dashboard com gráficos mais detalhados;
- Página de calibração dos sensores;
- Testes automatizados para módulos de lógica;
- Documentação com fotos do circuito físico.

---

## 🎓 Contexto acadêmico

Projeto desenvolvido no contexto da disciplina **Performance em Sistemas Ciberfísicos**, aplicando conceitos de sistemas embarcados, IoT, multitarefa, análise de desempenho e integração entre componentes físicos e digitais.

---

## 👥 Integrantes

- Alan Mendes Lopes Vareschini;
- Carlos Henrique Viana Marinho;
- José Ricardo Zella Aquino de Paula;
- Rafael Berton Martins;
- Rafael Padilha de Lima Mattioli.

---

## 📄 Licença

Este projeto foi desenvolvido para fins acadêmicos.

Caso deseje disponibilizá-lo como software livre, recomenda-se adicionar uma licença como MIT, Apache 2.0 ou GPL-3.0.

---

<p align="center">
  Desenvolvido com ESP32, C++ e foco em sistemas ciberfísicos.
</p>
