#include "OLED_2in23.h"
#include "GUI_Paint.h"
#include "pico/stdlib.h"
#include <string.h>
#include <stdlib.h>
// update_screen is a little file I made to control the ui
// rather than include the kinda lengthy functions in this
#include "update_screen.h"
#include "hardware/gpio.h"
#include "hardware/flash.h"

// Just an enum for a type of buttonpress
enum EventType {
    None = 0,
    Enter = 1,
    Leave = 2,
    Reject = 3
};

// A custom datatype that stores a button press
struct Event {
    enum EventType eventType;
    uint64_t timestamp;
};

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

// Test values. Do change them. Or delete them eventually.
// (Not the variables themselves, we need those. Just the numbers.)
int currentCap = 0;
int currentRejected = 0;

// Debouncing delay time, in milliseconds. 50 seems like a good value.
const int delayTime = 50;

// This variable is automatically assigned the memory location
// of the end of the program on the flash storage. I.E. it
// signifies the first place that's safe to write.
extern char* __flash_binary_end;

// This stores the timestamp of the previous button press.
// It's used for debouncing.
unsigned long timePressed;

// This stores the current page number that the program should
// write to, starting from 0 at the end of the program itself.
unsigned int currentPage;

// This stores the first safe write location, as an offset into
// the flash memory rather than an absolute address.
int writeStartLoc;

/**
 * Routine to write a page to the flash memory
 */
void writePage(struct Event eventToWrite) {
    int writeLoc = writeStartLoc + (currentPage * FLASH_PAGE_SIZE);
    void* writeBuffer = malloc(FLASH_PAGE_SIZE);
    memcpy(writeBuffer, &eventToWrite, sizeof(struct Event));
    flash_range_program(writeLoc, writeBuffer, FLASH_PAGE_SIZE);
    currentPage++;
    return;
}

/**
 * Routine to read a page from memory
 */
struct Event *readPage(int pageNumber) {
    int readLoc = XIP_BASE + writeStartLoc + (pageNumber * FLASH_PAGE_SIZE);
    struct Event *readEvent = (struct Event *)readLoc;
    return readEvent;
}

/**
 * This is the callback funciton to update the display periodically
 * parameters: *t - the timer calling the function
 */
bool screenCallback(struct repeating_timer *t) {
    updateScreen(canvas, currentCap, currentRejected);
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
                currentCap++;
                break;
            case DECREASE_PIN:
                buttonPress.eventType = Leave;
                currentCap--;
                break;
            case REJECT_PIN:
                buttonPress.eventType = Reject;
                currentRejected++;
                break;
        }
        buttonPress.timestamp = to_ms_since_boot(get_absolute_time())/1000;
        rewriteNumbers(canvas, currentCap, currentRejected);
    } else if (events == GPIO_IRQ_EDGE_RISE) {
        timePressed = to_ms_since_boot(get_absolute_time());
    }
    return;
}

/**
 * Routine to find the number of pages that have been written/
 * the number of the next free page.
 */
int findCurrentPage(int thisPage) {
    struct Event* thisEvent = readPage(thisPage);
    return ((*thisEvent).eventType == None) ? thisPage : findCurrentPage(thisPage + 1);
}

/*
 * Main funciton to execute when the microcontroller receives power
 */
int main() {
    // Initialize the hardware
    stdio_init_all();
    DEV_Module_Init();
    OLED_2in23_Init();

    // Initialize variables
    uintptr_t flash_end = (uintptr_t) &__flash_binary_end;
    writeStartLoc = (flash_end + (4096 - (flash_end % 4096)) - XIP_BASE);

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

