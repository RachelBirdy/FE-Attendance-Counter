#include "pico/stdlib.h"
#include <string.h>
#include <stdlib.h>
// update_screen is a little file I made to control the ui
// rather than include the kinda lengthy functions in this
#include "update_screen.h"
// writer is a little library to make reading from and writing
// to the flash easier
#include "writer.h"
#include "GPIO_Buttons.h"

// This is the pointer to the canvas that the various ui
// elements are drawn onto, and is passed into the various
// graphical functions.
UBYTE *canvas;

// How many milliseconds to wait between screen updates.
// Number is negative to indicate it should wait 250ms
// in total, rather than 250ms after the previous screen
// update command finishes executing.
const int ANIMATION_DELAY = -250;

struct Status *currentStatus;

/**
 * This is the callback funciton to update the display periodically
 * parameters: *t - the timer calling the function
 */
bool screenCallback(struct repeating_timer *t) {
    updateScreen(canvas, currentStatus->cap, currentStatus->rejected);
    return true;
}

void gpio_callback(uint gpio, uint32_t events) {
    gpio_pressed(gpio, events, currentStatus, canvas);
    return;
}

/*
 * Main funciton to execute when the microcontroller receives power
 */
int main() {
    currentStatus = calloc(1, sizeof(struct Status));

    // Initialize the hardware
    stdio_init_all();
    DEV_Module_Init();
    OLED_2in23_Init();
    writer_init(currentStatus);

    // Prepare the display canvas to be drawn on
    UWORD Imagesize = ((OLED_2in23_WIDTH%8==0)? (OLED_2in23_WIDTH/8): (OLED_2in23_WIDTH/8+1)) * OLED_2in23_HEIGHT;
    canvas = (UBYTE *) malloc(Imagesize);
    Paint_NewImage(canvas, OLED_2in23_WIDTH, OLED_2in23_HEIGHT, 0, BLACK);
    Paint_SelectImage(canvas);

    // Set up the display update callback
    struct repeating_timer timer;
    add_repeating_timer_ms(ANIMATION_DELAY, screenCallback, NULL, &timer);

    // Set up the gpio interupts
    timePressed = to_ms_since_boot(get_absolute_time());
    if (PULL_UP_PRESENT) {
        gpio_disable_pulls(0);
        gpio_disable_pulls(1);
        gpio_disable_pulls(2);
    }
    gpio_set_irq_enabled_with_callback(0, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(1, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(2, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    while(true); // ATTENDANCE COUNTING IS ETERNAL.
    return 0;
}

