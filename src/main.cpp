#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "gui.h"

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
int step = 1;
bool playing = false;
volatile unsigned long lastTime = 0;

const char* data[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

GUI gui(DISP1_CS, DISP1_DC, DISP1_RESET, DISP2_CS, DISP2_DC, DISP2_RESET);

#define CLK 21
#define DT  20
#define SW  19

// ISR für Drehung
void updateEncoder() {
    bool dt = digitalRead(DT);
    currentEventUp = (dt == LOW);  // Richtung bestimmen
    updateEvent = true;
}

// ISR für Button
void handleButton() {
    gui.printCursor(1, step);
    step = 1;
    playing = !playing;
}

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
        step = 1;
        while (step <= 32) {
            if (!playing) {
                step = 1;
                break;
            }
            if (step == 1) {
                gui.printCursor(step, 32);
            } else {
                gui.printCursor(step, step - 1);
            }
            delay(125);
            step++;
        }
    } else {
        if (updateEvent) {
            Serial.println("Encoder Update");
            int old_step = step;
            if (currentEventUp && step != 32) {
                Serial.println("Drehung rechts");
                step += 1;
            } else if (!currentEventUp && step != 1) {
                Serial.println("Drehung links");
                step -= 1;
            }
            Serial.println(String(step));
            gui.printCursor(step, old_step);
            updateEvent = false;
        }
    }
}