#include "hardware/gpio.h"
#include "structs.h"
#include "DEV_Config.h"
#include "writer.h"

// Do your buttons have pull up resistors?
const bool PULL_UP_PRESENT = true;

// The gpio pins your buttons are connected to
const int INCREASE_PIN = 1;
const int DECREASE_PIN = 2;
const int REJECT_PIN = 0;

// Debouncing delay time, in milliseconds. 50 seems like a good value.
const int delayTime = 50;

// This stores the timestamp of the previous button press.
// It's used for debouncing.
unsigned long timePressed;

/**
 * This is the callback function for when a button is pressed
 */
void gpio_pressed(uint gpio, uint32_t events, struct Status *currentStatus, UBYTE *canvas) {
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
        rewriteNumbers(canvas, currentStatus->cap, currentStatus->rejected);
    } else if (events == GPIO_IRQ_EDGE_RISE) {
        timePressed = to_ms_since_boot(get_absolute_time());
    }
    return;
}

void GPIO_init() {
    timePressed = to_ms_since_boot(get_absolute_time());
    if (PULL_UP_PRESENT) {
        gpio_disable_pulls(0);
        gpio_disable_pulls(1);
        gpio_disable_pulls(2);
    }
}
