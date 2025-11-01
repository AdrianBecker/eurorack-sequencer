#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <gui.h>>

#define DISP1_CS       43
#define DISP2_CS       47
#define DISP1_DC       40
#define DISP2_DC       48
#define DISP1_RESET    42
#define DISP2_RESET    49
#define DISP1_LED       8
#define DISP2_LED       7

volatile bool currentEventUp = false;
volatile bool updateEvent = false;
int entry = 1;
bool playing = false;
volatile unsigned long lastTime = 0;

const char* data[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

GUI gui(DISP1_CS, DISP1_DC, DISP1_RESET, DISP2_CS, DISP2_DC, DISP2_RESET);

#define CLK 21
#define DT  20
#define SW  19

void setup() {
    Serial.begin(9600);

    pinMode(DISP1_LED, OUTPUT);
    analogWrite(DISP1_LED, 255);
    pinMode(DISP2_LED, OUTPUT);
    analogWrite(DISP2_LED, 255);

    gui.begin();

    // pinMode(CLK, INPUT_PULLUP);
    // pinMode(DT, INPUT_PULLUP);
    // pinMode(SW, INPUT_PULLUP);

    pinMode(CLK, INPUT);
    pinMode(DT, INPUT);
    pinMode(SW, INPUT);

    attachInterrupt(digitalPinToInterrupt(CLK), updateEncoder, RISING);
    attachInterrupt(digitalPinToInterrupt(SW), handleButton, RISING);
}

void loop() {
    if (playing) {
        entry = 1;
        while (entry <= 32) {
            if (!playing) {
                entry = 1;
                break;
            }
            if (entry == 1) {
                gui.printCursor(entry, 32);
            } else {
                gui.printCursor(entry, entry - 1);
            }
            delay(125);
            entry++;
        }
    } else {
        if (updateEvent) {
            Serial.println("Encoder Update");
            int old_entry = entry;
            if (currentEventUp && entry != 32) {
                Serial.println("Drehung rechts");
                entry += 1;
            } else if (!currentEventUp && entry != 1) {
                Serial.println("Drehung links");
                entry -= 1;
            }
            Serial.println(String(entry));
            gui.printCursor(entry, old_entry);
            updateEvent = false;
        }
    }
}

// ISR für Drehung
void updateEncoder() {
    bool dt = digitalRead(DT);
    currentEventUp = (dt == LOW);  // Richtung bestimmen
    updateEvent = true;
}

// ISR für Button
void handleButton() {
    gui.printCursor(1, entry);
    entry = 1;
    playing = !playing;
}