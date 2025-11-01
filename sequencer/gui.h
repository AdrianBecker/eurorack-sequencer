#ifndef GUI_H
#define GUI_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#define DISP_WIDTH    128
#define DISP_HEIGHT   128
#define DISP_HPADDING   8
#define DISP_VPADDING   8
#define DISP_CPADDING   4
#define DISP_LPADDING   5
#define DISP_SPACER     6
#define DISP_LSPACER   10
#define TEXT_SIZE       2
    // 1	 6 ×  8
    // 2	12 × 16
    // 3	18 × 24
    // 4	24 × 32
    // 5	30 × 40

extern const char* data[12];

class GUI {
    private:
        struct CursorPos {
            int x;
            int y;
            bool disp1;
        };

        struct SeqPos {
            int row;
            int col;
        };

        static const int widths[5];
        static const int heights[5];

        static const int ROWS;
        static const int COLS;

        int seqMatrix[ROWS][COLS];

        Adafruit_ST7735 tft1;
        Adafruit_ST7735 tft2;

    public:
        GUI(int8_t CS1, int8_t DC1, int8_t RST1, int8_t CS2, int8_t DC2, int8_t RST2) : tft1(CS1, DC1, RST1), tft2(CS2, DC2, RST2);

        void begin();
        void clear();
        void write(int step, String text);
        void writeEntry(int step, int data);
        int getEntry(int step);
        String getNote(int step);
        void loadSeq();
        void printSeq();
        void printCursor(int new_step, int old_step);
        void printCosmetics(Adafruit_ST7735* tft);
        void printLines(Adafruit_ST7735* tft);
        CursorPos determineCursorPos(int step);
        SeqPos determineSeqPos(int step);
        int determineStep(int row, int col);
};

#endif