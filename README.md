# 🗡️ ESP32 Zelda Polyphonic Buzzer Music Player

Este projeto reproduz a música tema clássica de **The Legend of Zelda: A Link to the Past (Super Nintendo)** em um **ESP32** utilizando **2 Buzzers Passivos** para criar um áudio **polifônico de 2 canais** (Melodia principal + Harmonia/Baixo simultâneos).

---

## 🛠️ Requisitos de Hardware

- **1x ESP32** (ESP32 Dev Module / NodeMCU ESP32)
- **2x Buzzers Passivos** (Piezoelétricos)
- **Jumpers e Protoboard**
- *(Opcional)* 2x Resistores de 100Ω a 220Ω (para balanceamento de volume entre os buzzers)

> [!IMPORTANT]
> **Buzzers Passivos vs Ativos**: Para tocar notas musicais (frequências variáveis), os buzzers **devem ser PASSIVOS**. Buzzers ativos apenas emitem um apito fixo em frequência constante.

---

## 🔌 Esquema de Ligação (Pinout)

| Componente | Pino do ESP32 | Função |
| :--- | :--- | :--- |
| **Buzzer 1 (Melodia Principal)** `(+)` | **GPIO 18** | Canal PWM 1 - Voz de Melodia |
| **Buzzer 1** `(-)` | **GND** | Terra |
| **Buzzer 2 (Harmonia / Acompanhamento)** `(+)` | **GPIO 19** | Canal PWM 2 - Voz de Harmonia |
| **Buzzer 2** `(-)` | **GND** | Terra |

```text
              ┌─────────────────────────┐
              │        ESP32            │
              │                         │
              │  [GPIO 18] ─── ( + ) Buzzer 1 (Melodia)
              │                 ( - ) GND
              │                         │
              │  [GPIO 19] ─── ( + ) Buzzer 2 (Harmonia)
              │                 ( - ) GND
              └─────────────────────────┘
```

---

## 🎼 Como Funciona a Polifonia no ESP32

O ESP32 possui o periférico **LEDC (LED Control / PWM)** que permite gerar ondas quadradas em frequências independentes em múltiplos pinos GPIO sem travar a CPU.

- **Voz 1 (GPIO 18)**: Executa a linha melódica do tema Hyrule Overworld.
- **Voz 2 (GPIO 19)**: Executa a linha de baixo e contraponto em sincronia absoluta.
- **Articulação**: Cada nota inclui um micro-intervalo de silêncio (staccato) entre as transições, simulando o efeito sonoro de sintetizadores 16-bits do SNES.
- **Compatibilidade**: O código possui suporte para as versões 2.x e 3.x do ESP32 Arduino Core (`ledcWriteTone` / `ledcAttach`).

---

## 🚀 Como Executar o Projeto com PlatformIO

### 1. Compilar o Projeto
```bash
pio run
```

### 2. Fazer Upload para o ESP32
```bash
pio run --target upload
```

### 3. Abrir o Monitor Serial (115200 baud)
```bash
pio device monitor
```

No Serial Monitor, você verá o andamento da música passo a passo:
```text
=============================================
   ZELDA: A LINK TO THE PAST - ESP32 MUSIC
=============================================
Buzzer 1 (Melodia) : GPIO 18
Buzzer 2 (Harmonia): GPIO 19
Iniciando reprodução polifônica...
=============================================

▶ Tocando Tema do Zelda...
[01/37] B1:  523 Hz | B2:  262 Hz | Dur: 720 ms
[02/37] B1:  392 Hz | B2:  196 Hz | Dur: 240 ms
...
```

---

## 📁 Estrutura do Repositório

```text
Zelda_ESP/
├── platformio.ini      # Configurações do ambiente de compilação PlatformIO
├── README.md           # Documentação completa do projeto
└── src/
    └── main.cpp        # Partitura polifônica, frequências e motor PWM LEDC
```
