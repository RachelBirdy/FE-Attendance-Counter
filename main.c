#include "OLED_2in23.h"
#include "GUI_Paint.h"
#include "pico/stdlib.h"
#include <strings.h>
#include <stdlib.h>
#include "update_screen.h"

UBYTE *canvas;

const int ANIMATION_DELAY = -250;
const int STARTING_CAP = 0;
const int CURRENT_CAP = 0;

int currentCap = 100;
int currentRejected = 25;

bool screenCallback(struct repeating_timer *t) {
    updateScreen(canvas, currentCap, currentRejected);
    return true;
}

int main() {
    // Init
    stdio_init_all();
    DEV_Module_Init();
    OLED_2in23_Init();
    UWORD Imagesize = ((OLED_2in23_WIDTH%8==0)? (OLED_2in23_WIDTH/8): (OLED_2in23_WIDTH/8+1)) * OLED_2in23_HEIGHT;
    if((canvas = (UBYTE *)malloc(Imagesize)) == NULL) {
        while(1){
            printf("Failed to apply for black memory...\r\n");
        }
    }

    Paint_NewImage(canvas, OLED_2in23_WIDTH, OLED_2in23_HEIGHT, 0, BLACK);
    printf("Drawing\r\n");
    //1.Select Image
    Paint_SelectImage(canvas);
    DEV_Delay_ms(20);

    struct repeating_timer timer;
    add_repeating_timer_ms(ANIMATION_DELAY, screenCallback, NULL, &timer);
    while(true);
    return 0;

}

