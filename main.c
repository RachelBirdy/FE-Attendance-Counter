#include "OLED_2in23.h"
#include "GUI_Paint.h"
#include "pico/stdlib.h"
#include <string.h>
#include <stdlib.h>
// update_screen is a little file I made to control the ui
// rather than include the kinda lengthy functions in this
#include "update_screen.h"
#include "hardware/gpio.h"
// writer is a little library to make reading from and writing
// to the flash easier
#include "writer.h"

// This is the pointer to the canvas that the various ui
// elements are drawn onto, and is passed into the various
// graphical functions.
UBYTE *canvas;

// Do your buttons have pull up resistors?
const bool PULL_UP_PRESENT = true;

// The gpio pins your buttons are connected to
const int INCREASE_PIN = 1;
const int DECREASE_PIN = 2;
const int REJECT_PIN = 0;

// How many milliseconds to wait between screen updates.
// Number is negative to indicate it should wait 250ms
// in total, rather than 250ms after the previous screen
// update command finishes executing.
const int ANIMATION_DELAY = -250;

// Initialization numbers. Don't change them.
const int STARTING_CAP = 0;
const int CURRENT_CAP = 0;

// Debouncing delay time, in milliseconds. 50 seems like a good value.
const int delayTime = 50;

// This stores the timestamp of the previous button press.
// It's used for debouncing.
unsigned long timePressed;

struct Status *currentStatus;

/**
 * This is the callback funciton to update the display periodically
 * parameters: *t - the timer calling the function
 */
bool screenCallback(struct repeating_timer *t) {
    updateScreen(canvas, currentStatus->cap, currentStatus->rejected);
    return true;
}

/**
 * This is the callback function for when a button is pressed
 */
void gpio_callback(uint gpio, uint32_t events) {
    if ((events == GPIO_IRQ_EDGE_FALL) && (to_ms_since_boot(get_absolute_time()) - timePressed > delayTime)) {
        struct Event buttonPress;
        switch(gpio) {
            case INCREASE_PIN:
                buttonPress.eventType = Enter;
                currentStatus->cap++;
                break;
            case DECREASE_PIN:
                buttonPress.eventType = Leave;
                currentStatus->cap--;
                break;
            case REJECT_PIN:
                buttonPress.eventType = Reject;
                currentStatus->rejected++;
                break;
        }
        buttonPress.timestamp = to_ms_since_boot(get_absolute_time())/1000;
        writePage(buttonPress);
        rewriteNumbers(canvas, currentStatus->cap, currentPage);
    } else if (events == GPIO_IRQ_EDGE_RISE) {
        timePressed = to_ms_since_boot(get_absolute_time());
    }
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

