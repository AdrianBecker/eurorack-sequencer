#include "gui.h"

const int GUI::widths[5] = {6, 12, 18, 24, 30};
const int GUI::heights[5] = {8, 16, 24, 32, 40};
const int GUI::ROWS = 4;
const int GUI::COLS = 8;

extern const char* data[12];

GUI::GUI(int8_t CS1, int8_t DC1, int8_t RST1, int8_t CS2, int8_t DC2, int8_t RST2) : tft1(CS1, DC1, RST1), tft2(CS2, DC2, RST2) {}

void begin() {
    // CS-Pins als Ausgang definieren
    pinMode(DISP1_CS, OUTPUT);
    pinMode(DISP2_CS, OUTPUT);

    // Beide CS-Pins HIGH starten
    digitalWrite(DISP1_CS, HIGH);
    digitalWrite(DISP2_CS, HIGH);

    // Display 1 initialisieren
    digitalWrite(DISP1_CS, LOW);      // Display 1 aktiv
    tft1.initR(INITR_144GREENTAB);
    digitalWrite(DISP1_CS, HIGH);     // wieder deaktivieren

    // Display 2 initialisieren
    digitalWrite(DISP2_CS, LOW);      // Display 2 aktiv
    tft2.initR(INITR_144GREENTAB);
    digitalWrite(DISP2_CS, HIGH);     // wieder deaktivieren

    tft1.setRotation(2);
    tft2.setRotation(2);

    tft1.fillScreen(ST77XX_BLACK);
    tft2.fillScreen(ST77XX_BLACK);

    tft1.setTextColor(ST77XX_WHITE);
    tft2.setTextColor(ST77XX_WHITE);
    //tft1.setTextSize(TEXT_SIZE);
    //tft2.setTextSize(TEXT_SIZE);

    this->printSeq();
    this->printCosmetics(&tft1);
    this->printCosmetics(&tft2);
    this->printCursor(1, 1);
    Serial.println(String(entry));
}

void clear() {
    tft1.fillScreen(ST77XX_BLACK);
    tft2.fillScreen(ST77XX_BLACK);
}

void write(int step, String text) {
    CursorPos pos = determineCursorPos(step);
    if (text.length() == 1) {
        pos.x += widths[TEXT_SIZE - 1] / 2;
    }

    if (pos.disp1) {
        tft1.setCursor(pos.x, pos.y);
        tft1.setTextSize(TEXT_SIZE);
        tft1.print(text);
    } else {
        tft2.setCursor(pos.x, pos.y);
        tft2.setTextSize(TEXT_SIZE);
        tft2.print(text);
    }
}

void writeEntry(int step, int data) {
    SeqPos pos = determineSeqPos(step);
    seqMatrix[pos.row][pos.col] = data;
}

int getEntry(int step) {
    SeqPos pos = determineSeqPos(step);
    return seqMatrix[pos.row - 1][pos.col - 1];
}

String getNote(int step) {
    int idx = this->getEntry(step);
    Serial.println("Step " + String(step) + " -> " + String(data[idx]));
    return String(data[idx]);
}

void loadSeq() {
    int idx = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            seqMatrix[i][j] = idx;
            Serial.println("i = " + String(i) + ", j = " + String(j) + " -> " + String(seqMatrix[i][j]));
            idx++;
            if (idx >= (sizeof(data) / sizeof(data[0]))) {
                idx = 0;
            }
        }
    }
    Serial.println("Matrix geladen");
}

void printSeq() {
    this->clear();
    this->loadSeq();
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int step = determineStep(i + 1, j + 1);
            this->write(step, this->getNote(step));
        }
    }
}

void printCursor(int new_step, int old_step) {
    CursorPos old_pos = determineCursorPos(old_step);
    CursorPos new_pos = determineCursorPos(new_step);

    if (new_pos.disp1) {
        tft1.drawRect(new_pos.x, new_pos.y, widths[TEXT_SIZE - 1] * 2, heights[TEXT_SIZE - 1], ST77XX_WHITE);
    } else {
        tft2.drawRect(new_pos.x, new_pos.y, widths[TEXT_SIZE - 1] * 2, heights[TEXT_SIZE - 1], ST77XX_WHITE);
    }

    if (old_pos.disp1) {
        tft1.drawRect(old_pos.x, old_pos.y, widths[TEXT_SIZE - 1] * 2, heights[TEXT_SIZE - 1], ST77XX_BLACK);
    } else {
        tft2.drawRect(old_pos.x, old_pos.y, widths[TEXT_SIZE - 1] * 2, heights[TEXT_SIZE - 1], ST77XX_BLACK);
    }
}

void printCosmetics(Adafruit_ST7735* tft) {
    tft->setTextSize(TEXT_SIZE - 1);

    int x = DISP_HPADDING;
    int offset = (2 * heights[TEXT_SIZE - 1] + DISP_LPADDING - heights[TEXT_SIZE - 2]) / 2;
    int y1 = this->determineCursorPos(1).y + offset;
            
    tft->fillRect(x - 1, y1 - 1, widths[TEXT_SIZE - 2] + 1, heights[TEXT_SIZE - 2] + 1, ST77XX_WHITE);
    Serial.println("Rechteck 1 gezeichnet auf x: " + String(x) + ", y: " + String(y1));
    tft->setTextColor(ST77XX_BLACK);
    tft->setCursor(x, y1);
    tft->print("1");
    Serial.println("Zeichen 1 gezeichnet auf x: " + String(x) + ", y: " + String(y1));

    int y2 = this->determineCursorPos(17).y + offset;
    tft->fillRect(x - 1, y2 - 1, widths[TEXT_SIZE - 2] + 1, heights[TEXT_SIZE - 2] + 1, ST77XX_WHITE);
    Serial.println("Rechteck 2 gezeichnet auf x: " + String(x) + ", y: " + String(y2));
    tft->setCursor(x, y2);
    tft->print("2");
    Serial.println("Zeichen 2 gezeichnet auf x: " + String(x) + ", y: " + String(y2));

    this->printLines(tft);
    tft->setTextColor(ST77XX_WHITE);
}

void printLines(Adafruit_ST7735* tft) {
    tft->setTextColor(ST77XX_WHITE);
    tft->setTextSize(TEXT_SIZE - 1);
    int offset = 0;
    for (int i = 0; i < 8; i++) {
        tft->setCursor(this->determineCursorPos(i + 1).x + offset, DISP_VPADDING);
        tft->print(i + 1);

    }

    for (int i = 8; i < 16; i++) {
        if ((i + 1) / 10 == 0) {
            offset = (widths[TEXT_SIZE - 1] - widths[TEXT_SIZE - 2]) / 2;
        }
        tft->setCursor(this->determineCursorPos(i + 1).x, this->determineCursorPos(9).y + heights[TEXT_SIZE - 1] + DISP_LSPACER);
        tft->print(i + 1);
    }
}

CursorPos determineCursorPos(int step) {
    SeqPos seq = determineSeqPos(step);
    CursorPos pos;

    pos.disp1 = true;

    pos.x = DISP_HPADDING + widths[TEXT_SIZE - 2] + DISP_SPACER;
    if (seq.col > 1) {
        pos.x += (widths[TEXT_SIZE - 1] * 2 + DISP_CPADDING) * (seq.col - 1);
    }
    if (seq.col > 4) {
        pos.disp1 = false;
        pos.x = DISP_HPADDING + widths[TEXT_SIZE - 2] + DISP_SPACER;
        pos.x += (widths[TEXT_SIZE - 1] * 2 + DISP_CPADDING) * (seq.col - 5);
    }

    pos.y = DISP_VPADDING + heights[TEXT_SIZE - 2] + DISP_SPACER;

    if (seq.row > 1) {
        pos.y += heights[TEXT_SIZE - 1] + DISP_LPADDING;
    }
    if (seq.row > 2) {
        pos.y += heights[TEXT_SIZE - 1] + DISP_LSPACER + heights[TEXT_SIZE - 2] + DISP_SPACER;
    }
    if (seq.row == 4) {
        pos.y += heights[TEXT_SIZE - 1] + DISP_LPADDING;
    }

    // pos.y += (heights[TEXT_SIZE - 1] + DISP_LPADDING) * (seq.row - 1);
    // if (seq.row > 2) {
    //     pos.y -= DISP_LPADDING;
    //     pos.y += DISP_LSPACER + heights[TEXT_SIZE - 2] + DISP_SPACER;
    //     pos.y += (heights[TEXT_SIZE - 1] + DISP_LPADDING) * (seq.row - 3);
    // }

    return pos;
}

SeqPos determineSeqPos(int step) {
    SeqPos pos;

    pos.row = (step - 1) / 8 + 1;
    pos.col = (step - 1) % 8 + 1;

    return pos;
}

int determineStep(int row, int col) {
    return (row - 1) * COLS + col;
}