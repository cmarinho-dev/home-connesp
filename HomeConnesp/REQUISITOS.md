# 📌 Projeto: Sistema Ciberfísico com ESP32 (Performance)

## 🧠 Visão Geral
Este projeto consiste no desenvolvimento de um **sistema ciberfísico real**, utilizando **ESP32**, integrando:

- Hardware (sensores e atuadores)
- Firmware (código embarcado)
- Software (interface web)

O foco principal é **desempenho, monitoramento e eficiência**.

---

## 🎯 Objetivo do Sistema

Criar um sistema capaz de:

- Monitorar desempenho em tempo real
- Coletar e armazenar dados de sensores
- Gerenciar consumo de energia
- Exibir informações via interface web
- Permitir configuração dinâmica (sem regravar firmware)

---

# ⚙️ Requisitos do Sistema

## ✅ 1. Requisitos Funcionais

### 🌐 Interface Web
O sistema deve possuir uma interface web embarcada contendo:

#### 📊 Página de Performance (OBRIGATÓRIA)
Deve mostrar:

- Uso de CPU
- Uso de memória:
  - Heap
  - Stack
  - Flash
  - PSRAM (se houver)
- Tempo de execução de pelo menos **5 funções principais**
- Dados dos sensores
- Informações de tasks/threads
- Status do Wi-Fi
- Gráficos (uso de recursos + séries temporais)

---

### 📜 Sistema de Logs
- Tipos de log:
  - Erro
  - Warning
  - Informação
- Funcionalidades:
  - Visualização
  - Exportação dos logs

---

### ⚙️ Área de Configuração (via web)
Permitir alterar:

- Frequência de leitura dos sensores
- Parâmetros do sistema
- Configuração do Wi-Fi (sem precisar regravar o código)

---

### 📄 Página "Sobre"
Deve conter:

- Nome da universidade
- Nome dos integrantes
- E-mail
- Link do GitHub

---

### 🔄 Changelog
- Histórico de alterações do sistema acessível pela interface

---

## 🚫 2. Requisitos Não Funcionais

### 💾 Persistência
- Armazenar dados por **mínimo de 24 horas**

---

### ⚡ Performance
- NÃO pode usar funções bloqueantes (ex: `delay()`)
- Deve usar:
  - Multitarefa (mais de uma task)
  - Interrupções de hardware

---

### 🔋 Energia
Implementar pelo menos um:

- Light Sleep
- Deep Sleep

---

### 💻 Linguagem
- C/C++ (recomendado)
**ou**
- MicroPython (com limitações de performance)

---

### 📂 Versionamento
- Código no GitHub (público)

---

## 🔌 3. Requisitos de Hardware

### 📥 Entradas
- Pelo menos **2 sensores**

### 📤 Saídas
- Pelo menos **2 atuadores**
⚠️ NÃO pode ser:
- LED
- Buzzer

---

### 🧱 Estrutura Física
- Montado em caixa
- Organização adequada
- Seguro e funcional

---

### ❗ Restrição
- NÃO pode usar:
  - Simuladores
  - Projetos apenas virtuais

---

# 📚 Fundamentação Teórica

O trabalho deve explicar:

- Sistemas ciberfísicos
- Concorrência e paralelismo
- Gerenciamento de memória
- RTOS (sistemas operacionais embarcados)
- Economia de energia em IoT
- Observabilidade (logs + métricas)

---

# 🛠️ Metodologia

Desenvolvimento deve ser:

- Incremental
- Iterativo

Etapas:

1. Planejamento
2. Desenvolvimento do firmware
3. Integração de hardware
4. Criação da interface web
5. Testes contínuos
6. Avaliação de desempenho

---

# 🏗️ Desenvolvimento do Sistema

Descrever:

- Arquitetura geral
- Organização do código
- Tasks (multithreading)
- Uso de interrupções
- Estratégias de energia
- Interface web
- Persistência de dados

---

# 🧪 Testes e Validação

## 🔬 Testes Isolados
- Sensores
- Atuadores
- Módulos de software

---

## 🔗 Testes de Integração
- Hardware + firmware
- Firmware + interface web
- Persistência de dados
- Consumo de energia

---

# 📊 Resultados

Apresentar:

- Métricas de desempenho
- Uso de recursos
- Dados coletados
- Evidências via interface

---

# 🧾 Conclusão

- O que funcionou
- Limitações
- Decisões técnicas
- Melhorias futuras

---

# 📖 Referências

- Seguir padrão **ABNT obrigatório**

---

# 📊 Critérios de Avaliação

| Critério | Peso |
|--------|------|
| Implementação técnica | 30% |
| Interface web | 20% |
| Persistência e logs | 15% |
| Hardware funcional | 15% |
| Energia e performance | 10% |
| Documentação (ABNT) | 10% |

---

## ❗ Critério Eliminatório

- Não funcionar fisicamente
- Uso de simulador
- Não saber explicar o projeto
- Projeto copiado

---

# 🧠 Resumo Rápido (para IA entender fácil)

Sistema com ESP32 que:

- Usa sensores + atuadores reais
- Mostra desempenho em uma página web
- Armazena dados por 24h
- Usa multitarefa + interrupções
- Economiza energia
- Permite configuração via navegador
- Registra logs do sistema
- Está versionado no GitHub
- Funciona fisicamente (não simulado)

---