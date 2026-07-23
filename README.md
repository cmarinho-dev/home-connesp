<div align="center">

# HomeConnesp

[Instalação](#instalação) • [Funcionalidades](#funcionalidades) • [Arquitetura](#arquitetura-do-sistema) • [API](#rotas-da-api) • [Contribuir](#integrantes)

![ESP32](https://img.shields.io/badge/ESP32-IoT-blue)
![Arduino](https://img.shields.io/badge/Arduino-Framework-00979D)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-Multitarefa-green)
![C++](https://img.shields.io/badge/C%2B%2B-Embedded-00599C)
![Status](https://img.shields.io/badge/status-em%20desenvolvimento-yellow)

</div>

---

### Sumário
- [Introdução](#introdução)
- [Objetivo](#objetivo)
- [Funcionalidades](#funcionalidades)
- [Arquitetura do Sistema](#arquitetura-do-sistema)
- [Pré-requisitos](#pré-requisitos)
- [Instalação](#instalação)
- [Rotas da API](#rotas-da-api)
- [Mapeamento de Pinos](#mapeamento-de-pinos)
- [Roadmap](#roadmap)
- [Contexto Acadêmico](#contexto-acadêmico)

# Introdução

**HomeConnesp** é um sistema ciberfísico de automação residencial construído em torno de um **ESP32**. Ele une sensores, atuadores, coleta de métricas de desempenho e um painel web embarcado — tudo rodando diretamente na placa, sem depender de um servidor externo.

O foco do projeto está em:

- Monitoramento em tempo real do ambiente;
- Controle inteligente de dispositivos físicos;
- Baixo consumo de energia;
- Registro persistente de eventos (logs);
- Visualização de métricas de performance do próprio firmware;
- Uma interface web leve, responsiva e acessível pelo navegador.

# Objetivo

O projeto busca demonstrar, na prática, conceitos de **IoT** e **sistemas embarcados** — como multitarefa com FreeRTOS, tratamento de interrupções de hardware, persistência local de dados e análise de performance — combinando hardware, firmware e uma interface web em uma única solução ciberfísica.

# Funcionalidades

### Sensores
- Detecção de movimento via sensor **PIR**;
- Monitoramento de abertura/fechamento de porta com **Reed Switch**;
- Leitura orientada a interrupções de hardware;
- Intervalo de leitura ajustável pela própria interface web.

### Atuadores
- Controle de **relé** para luz, ventilador ou outra carga externa;
- Controle de **servo motor**, simulando uma tranca;
- Modo manual, operado pela interface web;
- Modo automático, guiado pelos sensores.

### Monitoramento de performance
- Estimativa de carga de CPU;
- Heap livre, heap mínimo e memória total;
- Informações de flash e do sketch gravado;
- Tempo de execução de funções críticas;
- Estado das tasks do FreeRTOS;
- Uptime do sistema.

### Logs
- Registro de eventos com níveis `INFO`, `WARN` e `ERROR`;
- Persistência local via **LittleFS**;
- Exportação dos logs em CSV;
- Limpeza de logs pela própria interface;
- Retenção aproximada de 24 horas.

### Interface web
- Dashboard com dados em tempo real;
- Gráficos construídos com **Chart.js**;
- Visual construído com **BeerCSS**;
- Controle manual de relé e servo;
- Configuração do intervalo dos sensores;
- Ativação/desativação de Light Sleep;
- Telas de logs e changelog.

# Lógica de Funcionamento

Com o modo automático ativo, o sistema reage sozinho aos eventos captados pelos sensores: movimento detectado pelo PIR aciona o relé, e o desligamento ocorre após um período de inatividade; a abertura da porta (Reed Switch) move o servo para a posição de destravamento, e o fechamento devolve o servo à posição de travamento. Todos os eventos relevantes são gravados no sistema de logs, e o ESP32 pode entrar em **Light Sleep** quando ocioso, para economizar energia.

# Arquitetura do Sistema

```
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

### Estrutura do projeto

```
home-connesp/
├── CONTEXTOS.md
└── HomeConnesp/
    ├── HomeConnesp.ino
    ├── Config.h / Config.cpp
    ├── Sensors.h / Sensors.cpp
    ├── Actuators.h / Actuators.cpp
    ├── Performance.h / Performance.cpp
    ├── Logger.h / Logger.cpp
    └── WebServer.h / WebServer.cpp
```

| Módulo | Responsabilidade |
| --- | --- |
| `HomeConnesp.ino` | Inicialização do sistema, criação das tasks FreeRTOS e configuração das interrupções |
| `Config` | Configurações persistidas em NVS/Preferences |
| `Sensors` | Leitura e estado dos sensores PIR e Reed Switch |
| `Actuators` | Controle do relé, do servo motor e do modo automático |
| `Performance` | Coleta de métricas de CPU, memória, flash, funções e tasks |
| `Logger` | Registro, persistência, exportação e limpeza de logs |
| `WebServer` | Servidor HTTP, interface web embarcada e rotas da API |

# Pré-requisitos

- **Placa ESP32** e cabo USB compatível;
- **Arduino IDE** (ou outro ambiente com suporte a ESP32);
- Bibliotecas: `ESPAsyncWebServer`, `AsyncTCP`, `ESP32Servo`;
- Uma rede Wi-Fi para o ESP32 se conectar.

# Instalação

Clone o repositório:

```sh
git clone https://github.com/cmarinho-dev/home-connesp.git
cd home-connesp
```

Abra a pasta `HomeConnesp` na Arduino IDE — o arquivo principal é `HomeConnesp/HomeConnesp.ino`.

No gerenciador de placas, instale o suporte ao **ESP32** e, em seguida, as bibliotecas `ESPAsyncWebServer`, `AsyncTCP` e `ESP32Servo`.

Configure as credenciais de Wi-Fi antes de compilar. Recomenda-se criar um arquivo `secrets.h` (e adicioná-lo ao `.gitignore`) com:

```cpp
// secrets.h
#define WIFI_SSID "NOME_DA_REDE"
#define WIFI_PASS "SENHA_DA_REDE"
```

Selecione a placa ESP32 e a porta serial corretas e envie o firmware. Após a inicialização, o IP obtido pela placa é exibido no monitor serial — acesse-o no navegador (ex: `http://192.168.0.120`) para abrir o dashboard.

# Rotas da API

| Método | Rota | Descrição |
| --- | --- | --- |
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

Exemplo — ajustar o servo (`POST /api/servo`):

```json
{
  "angle": 180
}
```

# Mapeamento de Pinos

| Componente | Pino ESP32 | Função |
| --- | --- | --- |
| Sensor PIR | GPIO 5 | Detecção de movimento |
| Reed Switch | GPIO 19 | Estado da porta |
| Relé | GPIO 21 | Controle de carga |
| Servo motor | GPIO 18 | Simulação de tranca |

> O relé é **active-low**: nível lógico baixo aciona o módulo.

# Segurança e Boas Práticas

Para uso além do ambiente acadêmico, vale considerar:

- Nunca versionar SSID/senha do Wi-Fi (mantenha-os em `secrets.h`, fora do Git);
- Adicionar autenticação à interface web;
- Reforçar a validação de dados recebidos pela API;
- Evitar expor o ESP32 diretamente à internet;
- Adicionar proteção contra acionamentos indevidos dos atuadores.

# Roadmap

- Autenticação para acesso ao painel web;
- Portal de configuração de Wi-Fi;
- Suporte a atualização OTA;
- Integração com MQTT e com Home Assistant;
- Histórico persistente de sensores;
- Dashboard com gráficos mais detalhados;
- Página de calibração dos sensores;
- Testes automatizados para os módulos de lógica.

# Contexto Acadêmico

Este projeto nasceu na disciplina **Performance em Sistemas Ciberfísicos**, aplicando conceitos de sistemas embarcados, IoT, multitarefa, análise de desempenho e integração entre componentes físicos e digitais.

**Integrantes:**
- Alan Mendes Lopes Vareschini
- Carlos Henrique Viana Marinho
- José Ricardo Zella Aquino de Paula
- Rafael Berton Martins
- Rafael Padilha de Lima Mattioli

<!--
# Licença
Projeto desenvolvido para fins acadêmicos. Para disponibilizá-lo como software livre, recomenda-se adicionar uma licença como MIT, Apache 2.0 ou GPL-3.0. -->

---

<div align="center">

Desenvolvido com ESP32, C++ e foco em sistemas ciberfísicos.

</div>
