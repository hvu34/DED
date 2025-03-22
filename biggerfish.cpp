
#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "macros.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "draw_helper.h"
#include "gameScreen.h"
#include "fish_models.h"

extern unsigned short *fb;

void redraw_main_menu(unsigned char *parlcd_mem_base, char menuoption, u_int16_t color)
{
    font_descriptor_t *fdes = &font_winFreeSystem14x16;
    int i, j;
    int ptr;
    if (menuoption == 0)
    {
        for (i = 0; i < LCD_HEIGHT / 2; i++)
        { // half the height, because we are writing 2x
            for (j = 0; j < LCD_WIDTH; j++)
            {
                parlcd_write_data2x(parlcd_mem_base, 0);
            }
        }
    }
    u_int16_t defaultcolor = 10000;

    u_int16_t color1 = defaultcolor;
    u_int16_t color2 = defaultcolor;

    // We want to pass 0 to draw the default mainmenu, then we select the desired ones with 1,2
    if (menuoption == 1)
    {
        color1 = color;
    }
    else if (menuoption == 2)
    {
        color2 = color;
    }
    int x = 100;
    int y = 80;
    char str[] = "Fish Game";
    char *ch = str;
    for (i = 0; i < 10; i++)
    {
        draw_char(x, y, fdes, *ch, defaultcolor, TEXT_SCALE);
        x += TEXT_SCALE * char_width(fdes, *ch);
        ch++;
    }
    x = 160;
    y = 140;
    char str1[] = "New Game";
    char *ch1 = str1;
    for (i = 0; i < 9; i++)
    {
        draw_char(x, y, fdes, *ch1, color1, TEXT_SCALE / 2);
        x += (TEXT_SCALE / 2) * char_width(fdes, *ch1);
        ch1++;
    }
    x = 190;
    y = 180;
    char str2[] = "Exit";
    char *ch2 = str2;

    for (i = 0; i < 5; i++)
    {
        draw_char(x, y, fdes, *ch2, color2, TEXT_SCALE / 2);
        x += (TEXT_SCALE / 2) * char_width(fdes, *ch2);
        ch2++;
    }

    for (ptr = 0; ptr < LCD_WIDTH * LCD_HEIGHT; ptr += 2)
    {
        uint32_t value_to_write = 0;
        value_to_write |= fb[ptr];
        value_to_write <<= 16;
        value_to_write |= fb[ptr + 1];
        parlcd_write_data2x(parlcd_mem_base, value_to_write);
    }
}

void black_screen(unsigned char *parlcd_mem_base)
{
    int ptr = 0;
    for (int i = 0; i < LCD_HEIGHT; i++)
    {
        for (int j = 0; j < LCD_WIDTH; j++)
        {
            int c = 0;
            fb[ptr] = c;
            parlcd_write_data(parlcd_mem_base, fb[ptr++]);
        }
    }
}

int main(int argc, char *argv[])
{
    fb = (unsigned short *)malloc(320 * LCD_WIDTH * 2);
    unsigned char *mem_base;
    unsigned char *parlcd_mem_base;
    mem_base = (unsigned char *)map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    /* If mapping fails exit with error code */
    if (mem_base == NULL)
        exit(1);
    parlcd_mem_base = (unsigned char *)map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (parlcd_mem_base == NULL)
        exit(1);

    parlcd_hx8357_init(parlcd_mem_base);
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    redraw_main_menu(parlcd_mem_base, 0, 10000);
    bool previous = false;

    while (1)
    {
        int r = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
        int left_knob_val = ((r >> 16) & 0xff);
        draw_fish_model(20, 20, &fish_models, 0, 10000, TEXT_SCALE);

        // if we are at exit, we break with black screen
        if ((!previous) && ((r & 0x4000000) != 0))
        {
            black_screen(parlcd_mem_base);
            exit(0);
            break;
        }
        // if we press new game, we start the game
        if (previous && ((r & 0x4000000) != 0))
        {
            GameScreen(parlcd_mem_base, mem_base);
            // after the game screen ends, we want to black out the screen and go back to the main menu
            black_screen(parlcd_mem_base);
            redraw_main_menu(parlcd_mem_base, 0, 10000);
        }
        // Currently selected "New game"
        if ((left_knob_val % 32) < 16)
        {
            if (!previous)
            {
                redraw_main_menu(parlcd_mem_base, 1, 50000);
            }
            previous = true;
            // Currently selected "Exit"
        }
        else if ((left_knob_val % 32) >= 16)
        {
            if (previous)
            {
                redraw_main_menu(parlcd_mem_base, 2, 50000);
            }
            previous = false;
        }
    }
    return 0;
}
