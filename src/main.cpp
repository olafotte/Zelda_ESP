#include <Arduino.h>

/*
  ====================================================================
  ZELDA: A LINK TO THE PAST (SNES) - TEMA PRINCIPAL (POLIFONIA 2 BUZZERS)
  ====================================================================
  Conexões do Hardware:
  - Buzzer 1 (Melodia Principal) : Pino GPIO 18 (+) e GND (-)
  - Buzzer 2 (Harmonia / Baixo)  : Pino GPIO 19 (+) e GND (-)

  * NOTA: Certifique-se de usar Buzzers PASSIVOS (Piezoelétricos).
  ====================================================================
*/

// Definindo o tempo base (BPM)
#define TEMPO_BPM 125
#define Q_NOTE (60000 / TEMPO_BPM)           // Semínima (Quarter note) ~480ms
#define H_NOTE (Q_NOTE * 2)                  // Mínima (Half note) ~960ms
#define W_NOTE (Q_NOTE * 4)                  // Semibreve (Whole note) ~1920ms
#define E_NOTE (Q_NOTE / 2)                  // Colcheia (Eighth note) ~240ms
#define S_NOTE (Q_NOTE / 4)                  // Semicolcheia (Sixteenth note) ~120ms
#define DQ_NOTE (Q_NOTE + E_NOTE)            // Semínima pontuada ~720ms
#define DE_NOTE (E_NOTE + S_NOTE)            // Colcheia pontuada ~360ms

// Tabela de Frequências (Hz)
#define REST 0

#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247

#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494

#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988

#define NOTE_C6  1047
#define NOTE_D6  1175
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_G6  1568
#define NOTE_A6  1760

// Configuração dos Pinos do ESP32
#define BUZZER_1_PIN 18  // Voz 1: Melodia
#define BUZZER_2_PIN 19  // Voz 2: Harmonia / Acompanhamento

#define CHAN_1 0
#define CHAN_2 1

struct NoteStep {
    uint16_t freq1;    // Frequência do Buzzer 1 (Melodia)
    uint16_t freq2;    // Frequência do Buzzer 2 (Harmonia)
    uint16_t duration; // Duração do passo em ms
};

// Partitura Polifônica do Tema de Zelda (SNES Overworld Theme em Dó Maior)
const NoteStep zeldaTheme[] = {
    // --- INTRODUÇÃO / FANFARRA ---
    {NOTE_C5,  NOTE_C4,  DQ_NOTE},
    {NOTE_G4,  NOTE_G3,  E_NOTE},
    {REST,     REST,     S_NOTE},
    {NOTE_C5,  NOTE_C4,  S_NOTE},
    {NOTE_C5,  NOTE_C4,  S_NOTE},
    {NOTE_D5,  NOTE_D4,  S_NOTE},
    {NOTE_E5,  NOTE_E4,  S_NOTE},
    {NOTE_F5,  NOTE_F4,  S_NOTE},
    {NOTE_G5,  NOTE_G4,  H_NOTE},

    // --- TEMA PRINCIPAL (PARTE A) ---
    {NOTE_G5,  NOTE_E4,  DQ_NOTE},
    {NOTE_G5,  NOTE_E4,  S_NOTE},
    {NOTE_A5,  NOTE_F4,  S_NOTE},
    {NOTE_B5,  NOTE_G4,  S_NOTE},
    {NOTE_C6,  NOTE_C5,  W_NOTE},

    {NOTE_C6,  NOTE_A4,  E_NOTE},
    {NOTE_C6,  NOTE_A4,  E_NOTE},
    {NOTE_C6,  NOTE_A4,  E_NOTE},
    {NOTE_A5,  NOTE_F4,  E_NOTE},
    {NOTE_F5,  NOTE_D4,  E_NOTE},
    {NOTE_G5,  NOTE_E4,  DQ_NOTE},
    {NOTE_F5,  NOTE_D4,  E_NOTE},
    {NOTE_E5,  NOTE_C4,  H_NOTE},

    {NOTE_E5,  NOTE_C4,  E_NOTE},
    {NOTE_F5,  NOTE_D4,  E_NOTE},
    {NOTE_G5,  NOTE_E4,  DQ_NOTE},
    {NOTE_F5,  NOTE_D4,  E_NOTE},
    {NOTE_E5,  NOTE_C4,  E_NOTE},
    {NOTE_D5,  NOTE_B3,  E_NOTE},
    {NOTE_E5,  NOTE_C4,  DQ_NOTE},
    {NOTE_D5,  NOTE_B3,  E_NOTE},
    {NOTE_C5,  NOTE_G3,  H_NOTE},

    // --- SUBIDA DE TENSÃO (PARTE B) ---
    {NOTE_F5,  NOTE_D4,  DQ_NOTE},
    {NOTE_F5,  NOTE_D4,  S_NOTE},
    {NOTE_G5,  NOTE_E4,  S_NOTE},
    {NOTE_A5,  NOTE_F4,  S_NOTE},
    {NOTE_B5,  NOTE_G4,  H_NOTE},

    {NOTE_G5,  NOTE_E4,  DQ_NOTE},
    {NOTE_G5,  NOTE_E4,  S_NOTE},
    {NOTE_A5,  NOTE_F4,  S_NOTE},
    {NOTE_B5,  NOTE_G4,  S_NOTE},
    {NOTE_C6,  NOTE_C5,  H_NOTE},

    {NOTE_D6,  NOTE_F5,  H_NOTE},
    {NOTE_C6,  NOTE_E5,  H_NOTE},
    {NOTE_B5,  NOTE_D5,  H_NOTE},
    {NOTE_C6,  NOTE_C5,  W_NOTE},

    // Pausa antes de repetir o loop
    {REST,     REST,     1000}
};

const size_t totalNotes = sizeof(zeldaTheme) / sizeof(NoteStep);

// Função de compatibilidade do motor PWM LEDC do ESP32 (Core v2.x e v3.x)
void setTone(uint8_t pin, uint8_t channel, uint16_t freq) {
    if (freq == REST) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcWriteTone(pin, 0);
#else
        ledcWriteTone(channel, 0);
#endif
    } else {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcWriteTone(pin, freq);
#else
        ledcWriteTone(channel, freq);
#endif
    }
}

void setupBuzzer(uint8_t pin, uint8_t channel) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttach(pin, 2000, 8);
#else
    ledcSetup(channel, 2000, 8);
    ledcAttachPin(pin, channel);
#endif
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n=============================================");
    Serial.println("   ZELDA: A LINK TO THE PAST - ESP32 MUSIC");
    Serial.println("=============================================");
    Serial.println("Buzzer 1 (Melodia) : GPIO 18");
    Serial.println("Buzzer 2 (Harmonia): GPIO 19");
    Serial.println("Iniciando reprodução polifônica...");
    Serial.println("=============================================\n");

    setupBuzzer(BUZZER_1_PIN, CHAN_1);
    setupBuzzer(BUZZER_2_PIN, CHAN_2);
}

void loop() {
    Serial.println("▶ Tocando Tema do Zelda...");

    for (size_t i = 0; i < totalNotes; i++) {
        uint16_t f1 = zeldaTheme[i].freq1;
        uint16_t f2 = zeldaTheme[i].freq2;
        uint16_t dur = zeldaTheme[i].duration;

        // Imprime nota atual no Serial Monitor
        Serial.printf("[%02d/%02d] B1: %4d Hz | B2: %4d Hz | Dur: %d ms\n", 
                      (int)i + 1, (int)totalNotes, f1, f2, dur);

        // Toca as duas frequências simultaneamente nos 2 buzzers
        setTone(BUZZER_1_PIN, CHAN_1, f1);
        setTone(BUZZER_2_PIN, CHAN_2, f2);

        // Duração da nota
        delay(dur * 0.90);

        // Breve articulação de staccato (silêncio) entre as notas para som mais limpo
        setTone(BUZZER_1_PIN, CHAN_1, REST);
        setTone(BUZZER_2_PIN, CHAN_2, REST);
        delay(dur * 0.10);
    }

    Serial.println("✔ Fim da música. Reiniciando em 3 segundos...\n");
    delay(3000);
}
