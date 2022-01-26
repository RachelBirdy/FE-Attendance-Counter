#include "OLED_2in23.h"
#include "GUI_Paint.h"
#include "pico/stdlib.h"
#include <strings.h>
#include <stdlib.h>
#include "hardware/flash.h"

UBYTE *canvas;

extern char* __flash_binary_end;

#define TOTAL_FLASH_SIZE 2097152

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

    Paint_Clear(BLACK);

    uintptr_t flash_end = (uintptr_t) &__flash_binary_end;
    char* string = calloc(128, sizeof(char));
    int writeLoc = 0;
    sprintf(string, "W loc:%d", writeLoc);
    Paint_DrawString_EN(1,1,string,&Font12,WHITE,BLACK);
    char* buffer = calloc(1, FLASH_PAGE_SIZE);
    sprintf(buffer, "%d",FLASH_PAGE_SIZE);
    flash_range_erase(writeLoc, TOTAL_FLASH_SIZE - writeLoc);
    char *p = (char *)XIP_BASE + writeLoc;
    Paint_DrawString_EN(1,12,p,&Font8,WHITE,BLACK);
    OLED_2in23_draw_bitmap(0,0,&canvas[0],128,32);
    sleep_ms(1000);

    while(true); // ATTENDANCE COUNTING IS ETERNAL.
    return 0;
}

