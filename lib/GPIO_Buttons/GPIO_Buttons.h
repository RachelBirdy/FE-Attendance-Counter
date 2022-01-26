#include "hardware/gpio.h"
#include "structs.h"
#include "DEV_Config.h"

// Do your buttons have pull up resistors?
const bool PULL_UP_PRESENT;

// The gpio pins your buttons are connected to
const int INCREASE_PIN;
const int DECREASE_PIN;
const int REJECT_PIN;

// Debouncing delay time, in milliseconds. 50 seems like a good value.
const int delayTime;

// This stores the timestamp of the previous button press.
// It's used for debouncing.
unsigned long timePressed;

/**
 * This is the callback function for when a button is pressed
 */
void gpio_pressed(uint gpio, uint32_t events, struct Status *currentStatus, UBYTE *canvas);
