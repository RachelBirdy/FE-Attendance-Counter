#include "OLED_2in23.h"
#include "GUI_Paint.h"
#include <strings.h>
#include <stdlib.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"

const char* TOP_LINE = "Cur. capacity: ";
const char* BOTTOM_LINE = "Rejected: ";

const int TOP_LEN = 15;
const int BOTTOM_LEN = 10;

int currentRotation;
const float conversion_factor = 3.3f / (1 << 12);
float currentVoltage;

// Fully visible helix
const unsigned char helixTop[28] = {
0b00000010, 0b01000000,
0b00001100, 0b00110000,
0b00010000, 0b00001000,
0b00100000, 0b00000100,
0b01000000, 0b00000010,
0b01000000, 0b00000010,
0b10000000, 0b00000001,
0b10000000, 0b00000001,
0b10000000, 0b00000001,
0b10000000, 0b00000001,
0b01000000, 0b00000010,
0b01000000, 0b00000010,
0b00100000, 0b00000100,
0b00011000, 0b00011000
};

const unsigned char helixMidFront[6] = {
0b00000000, 0b01100000,
0b00000001, 0b10000000,
0b00000110, 0b00000000
};

const unsigned char helixMidBack[6] = {
0b00000110, 0b00000000,
0b00000001, 0b10000000,
0b00000000, 0b01100000
};

const unsigned char helixBottom[26] = {
0b00011000, 0b00011000,
0b00100000, 0b00000100,
0b01000000, 0b00000010,
0b01000000, 0b00000010,
0b10000000, 0b00000001,
0b10000000, 0b00000001,
0b10000000, 0b00000001,
0b10000000, 0b00000001,
0b01000000, 0b00000010,
0b01000000, 0b00000010,
0b00100000, 0b00000100,
0b00011000, 0b00011000,
0b00000100, 0b00100000
};

// Quarter view helix

const unsigned char helixHalfTop[14] = {
0b00011000,
0b00100100,
0b01000010,
0b01000010,
0b10000001,
0b10000001,
0b10000001,
0b10000001,
0b10000001,
0b10000001,
0b10000001,
0b10000001,
0b01000010,
0b01100110
};

const unsigned char helixHalfMidFront[3] = {
0b00101100,
0b00011000,
0b00110100
};

const unsigned char helixHalfMidBack[3] = {
0b00110100,
0b00011000,
0b00101100
};

const unsigned char helixHalfBottom[13] = {
0b01100110,
0b01000010,
0b10000001,
0b10000001,
0b10000001,
0b10000001,
0b10000001,
0b10000001,
0b10000001,
0b10000001,
0b01000010,
0b01100110,
0b00100100
};

const unsigned char helixSide[30] = {
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b00011000
};

void paintFront() {
    Paint_BmpWindows(0, 1, &helixTop[0], 16, 14);
    Paint_BmpWindows(0, 15, &helixMidFront[0], 16, 3);
    Paint_BmpWindows(0, 18, &helixBottom[0], 16, 13);
}

void paintHalfFront() {
    Paint_BmpWindows(4, 1, &helixHalfTop[0], 8, 14);
    Paint_BmpWindows(4, 15, &helixHalfMidFront[0], 8, 3);
    Paint_BmpWindows(4, 18, &helixHalfBottom[0], 8, 13);
}

void paintSide() {
    Paint_BmpWindows(4, 1, &helixSide[0], 8, 30);
}

void paintHalfBack() {
    Paint_BmpWindows(4, 1, &helixHalfTop[0], 8, 14);
    Paint_BmpWindows(4, 15, &helixHalfMidBack[0], 8, 3);
    Paint_BmpWindows(4, 18, &helixHalfBottom[0], 8, 13);
}

void paintBack() {
    Paint_BmpWindows(0, 1, &helixTop[0], 16, 14);
    Paint_BmpWindows(0, 15, &helixMidBack[0], 16, 3);
    Paint_BmpWindows(0, 18, &helixBottom[0], 16, 13);
}

void drawAnimation(UBYTE *canvas) {
    switch (currentRotation) {
        case 0:
            paintFront();
            break;
        case 1:
            paintHalfFront();
            break;
        case 2:
            paintSide();
            break;
        case 3:
            paintHalfBack();
            break;
        case 4:
            paintBack();
            break;
        case 5:
            paintHalfBack();
            break;
        case 6:
            paintSide();
            break;
        case 7:
            paintHalfFront();
            break;
    }
}

void rewriteNumbers(UBYTE *canvas, int currentCap, int currentRejected) {
    char capString[10];
    char rejString[10];
    itoa(currentCap, capString, 10);
    itoa(currentRejected, rejString, 10);
    Paint_Clear(BLACK);
    currentVoltage = (adc_read() * conversion_factor);
    char voltString[10];
    sprintf(voltString,"%f",currentVoltage);

    drawAnimation(canvas);

    Paint_DrawString_EN(16, 6, TOP_LINE, &Font8, WHITE, BLACK);
    Paint_DrawString_EN(41, 22, BOTTOM_LINE, &Font8, WHITE, BLACK);
    Paint_DrawString_EN(87, 4, capString, &Font12, WHITE, BLACK);
    Paint_DrawString_EN(87, 20, rejString, &Font12, WHITE, BLACK);
    Paint_DrawString_EN(100,1,voltString,&Font8, WHITE,BLACK);
    OLED_2in23_draw_bitmap(0,0,&canvas[0],128,32);
    return;
}

void updateScreen(UBYTE *canvas, int currentCap, int currentRejected) {
    (currentRotation == 7) ? currentRotation = 0 : currentRotation++;
    rewriteNumbers(canvas, currentCap, currentRejected);
    return;
}

void screen_init() {
    currentRotation = 0;
    stdio_init_all();
    adc_init();
    adc_gpio_init(29);
    adc_select_input(3);
    return;
}
