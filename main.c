#include "OLED_2in23.h"
#include "GUI_Paint.h"
#include "pico/stdlib.h"
#include <strings.h>
#include <stdlib.h>
#include "update_screen.h"

UBYTE *canvas;

// How many milliseconds to wait between screen updates.
const int ANIMATION_DELAY = -250;

// Initialization numbers. Don't change them.
const int STARTING_CAP = 0;
const int CURRENT_CAP = 0;

// Test values. Do change them. Or delete them eventually.
// (Not the variables themselves, we need those. Just the numbers.)
int currentCap = 100;
int currentRejected = 25;

/**
 * This is the callback funciton to update the display periodically
 * parameters: *t - the timer calling the function
 */
bool screenCallback(struct repeating_timer *t) {
    updateScreen(canvas, currentCap, currentRejected);
    return true;
}

/*
 * Main funciton to execute when the microcontroller receives power
 */
int main() {
    // Initialize the hardware
    stdio_init_all();
    DEV_Module_Init();
    OLED_2in23_Init();

    // Prepare the display canvas to be drawn on
    UWORD Imagesize = ((OLED_2in23_WIDTH%8==0)? (OLED_2in23_WIDTH/8): (OLED_2in23_WIDTH/8+1)) * OLED_2in23_HEIGHT;
    canvas = (UBYTE *) malloc(Imagesize);
    Paint_NewImage(canvas, OLED_2in23_WIDTH, OLED_2in23_HEIGHT, 0, BLACK);
    Paint_SelectImage(canvas);

    // Set up the display update callback
    struct repeating_timer timer;
    add_repeating_timer_ms(ANIMATION_DELAY, screenCallback, NULL, &timer);
    while(true); // ATTENDANCE COUNTING IS ETERNAL.
    return 0;
}

