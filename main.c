#include "OLED_2in23.h"
#include "GUI_Paint.h"
#include "pico/stdlib.h"
#include <strings.h>
#include <stdlib.h>
#include "update_screen.h"
#include "hardware/gpio.h"

UBYTE *canvas;

// Do your buttons have pull up/down resistors?
const bool PULL_UP_PRESENT = true;

// The gpio pins your buttons are connected to
const int INCREASE_PIN = 1;
const int DECREASE_PIN = 2;
const int REJECT_PIN = 0;

// How many milliseconds to wait between screen updates.
const int ANIMATION_DELAY = -250;

// Initialization numbers. Don't change them.
const int STARTING_CAP = 0;
const int CURRENT_CAP = 0;

// Test values. Do change them. Or delete them eventually.
// (Not the variables themselves, we need those. Just the numbers.)
int currentCap = 0;
int currentRejected = 0;

// Debouncing delay time, in milliseconds
const int delayTime = 50;

// Misc values
unsigned long timePressed;


/**
 * This is the callback funciton to update the display periodically
 * parameters: *t - the timer calling the function
 */
bool screenCallback(struct repeating_timer *t) {
    updateScreen(canvas, currentCap, currentRejected);
    return true;
}

void gpio_callback(uint gpio, uint32_t events) {
    if ((events == GPIO_IRQ_EDGE_FALL) && (to_ms_since_boot(get_absolute_time()) - timePressed > delayTime)) {
        switch(gpio) {
            case INCREASE_PIN:
                currentCap++;
                break;
            case DECREASE_PIN:
                currentCap--;
                break;
            case REJECT_PIN:
                currentRejected++;
                break;
        }
        rewriteNumbers(canvas, currentCap, currentRejected);
    } else if (events == GPIO_IRQ_EDGE_RISE) {
        timePressed = to_ms_since_boot(get_absolute_time());
    }
    return;
}

void gpio_release_callback(uint gpio, uint32_t events) {
    timePressed = to_ms_since_boot(get_absolute_time());
    return;
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

