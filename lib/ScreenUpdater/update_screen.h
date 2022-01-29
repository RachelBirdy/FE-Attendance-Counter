#include "OLED_2in23.h"
#include "GUI_Paint.h"
#include <strings.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"

void updateScreen(UBYTE *canvas, int currentCap, int currentRejected);

void rewriteNumbers(UBYTE *canvas, int currentCap, int currentRejected);
