/* This is just a test, to give the compiler something to do */
/* Obviously delete this file! */

#include <arch/zxn.h>
#include <z80.h>
#include <input.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <intrinsic.h>
#include <arch/zxn/esxdos.h>
#include <lib/zxn/libchai-layer2.h>

#include "banking.h"
#include "bmprint.h"
#include "copper.h"

#define BANK_IMAGE_LOADER 16
#define BANK_IMAGE_MAIN1 BANK_IMAGE_LOADER+3
#define BANK_IMAGE_MAIN2 BANK_IMAGE_MAIN1+3
#define BANK_IMAGE_MAIN BANK_IMAGE_MAIN2+3
#define BANK_FONT1 BANK_IMAGE_MAIN + 3

// gradient bars
uint16_t gradientBars[] = {
    // Reset at top
    CU_WAIT(0,0),
    CU_MOVE(REG_PALETTE_INDEX, 0x10),
    CU_MOVE(REG_PALETTE_VALUE_8, 0),
    // Start of bars
    // Three bars, each with 8 stripes
    // Each stripe needs three commands - wait, move index, move palette
    // BAR 1 - yellow-orange
    CU_WAIT(16,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 252),
    CU_WAIT(18,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 244),
    CU_WAIT(20,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 236),
    CU_WAIT(22,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 228),
    CU_WAIT(24,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 228),
    CU_WAIT(26,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 236),
    CU_WAIT(28,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 244),
    CU_WAIT(30,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 252),
    // BAR 2 - blues
    CU_WAIT(32,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 95),
    CU_WAIT(34,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 87),
    CU_WAIT(36,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 79),
    CU_WAIT(38,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 71),
    CU_WAIT(40,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 71),
    CU_WAIT(42,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 79),
    CU_WAIT(44,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 87),
    CU_WAIT(46,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 95),
    // BAR 3 - greens
    CU_WAIT(48,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 28),
    CU_WAIT(50,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 20),
    CU_WAIT(52,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 16),
    CU_WAIT(54,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 12),
    CU_WAIT(56,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 12),
    CU_WAIT(58,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 16),
    CU_WAIT(60,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 20),
    CU_WAIT(62,0),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 28),

    // Set border back to black
    CU_WAIT(64,0),
    CU_MOVE(REG_PALETTE_INDEX, 0x10),
    CU_MOVE(REG_PALETTE_VALUE_8, 0),

    CU_WAIT(400,0)
};

uint16_t parallaxScroll[] = {
    CU_WAIT(0,48),
    CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 3),
    CU_MOVE(REG_LAYER_2_OFFSET_X, 0),
    CU_WAIT(16,48),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 7),
    CU_WAIT(36,48),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 11),
    CU_WAIT(57,48),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 15),
    CU_WAIT(60,0),
    CU_MOVE(REG_LAYER_2_OFFSET_X, 0),
    CU_WAIT(80,48),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 19),
    CU_WAIT(106,48),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 23),
    CU_WAIT(120,48),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 27),
    CU_WAIT(160,48),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 28),
    CU_MOVE(REG_LAYER_2_OFFSET_X, 0),
    CU_WAIT(168,48),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 24),
    CU_WAIT(172,48),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 20),
    CU_WAIT(176,48),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 16),
    CU_WAIT(180,48),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 12),
    CU_WAIT(184,48),CU_MOVE(REG_PALETTE_INDEX, 0x10),CU_MOVE(REG_PALETTE_VALUE_8, 8),

    // Set border back to black
    CU_WAIT(192,48),
    CU_MOVE(REG_PALETTE_INDEX, 0x10),
    CU_MOVE(REG_PALETTE_VALUE_8, 0),

    CU_WAIT(400,0)
};

void OpenWindow() {
    IO_NEXTREG_REG = 0x1C;  // Reset clip window
    IO_NEXTREG_DAT = 0x01;

    for (uint8_t i = 0; i < 184; i++) {
        IO_NEXTREG_REG = 0x18;
        IO_NEXTREG_DAT = 0x0;   // Xbeg
        IO_NEXTREG_DAT = 0xFF;   // Xend
        IO_NEXTREG_DAT = i;   // Ybeg
        IO_NEXTREG_DAT = i+8;   // Yend
        WAIT_FOR_SCANLINE(192);
    }

    for (uint8_t i = 0; i < 60; i++)
        WAIT_FOR_SCANLINE(192);

    for (uint8_t i = 184; i != 0; i--) {
        IO_NEXTREG_REG = 0x18;
        IO_NEXTREG_DAT = 0x0;   // Xbeg
        IO_NEXTREG_DAT = 0xFF;   // Xend
        IO_NEXTREG_DAT = i;   // Ybeg
        IO_NEXTREG_DAT = 191;   // Yend
        WAIT_FOR_SCANLINE(192);
    }
}

void loadImage(char *filename, uint8_t bank)
{
    errno = 0;
    uint16_t filehandle = esxdos_f_open(filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);
    if (errno != ESXDOS_OK) {
        zx_border(INK_RED);
        return;
    }

    // Bank 16 into slot 2
    bank_set_16k(2, bank);

    uint8_t *addr = (uint8_t *)(0x4000);
    esxdos_f_read(filehandle, (uint8_t *)addr, 16384);

    bank_set_16k(2, bank + 1);
    addr = (uint8_t *)(0x4000);
    esxdos_f_read(filehandle, (uint8_t *)addr, 16384);

    bank_set_16k(2, bank + 2);
    addr = (uint8_t *)(0x4000);
    esxdos_f_read(filehandle, (uint8_t *)addr, 16384);

    esxdos_f_close(filehandle);
}

void DMACopyScreen(uint8_t bank)
{
    bank_set_16k(2, bank);
    
    IO_123B = 0x07;

    // DMA
    // WR0
    IO_6B = 0x7D;
    IO_6B = 0x00;
    IO_6B = 0x40; // A Start
    IO_6B = 0x00;
    IO_6B = 0x40; // 16k
    // WR1
    IO_6B = 0x14;
    // WR2
    IO_6B = 0x10;
    // WR4
    IO_6B = 0xCD;
    IO_6B = 0x00;
    IO_6B = 0x00;
    // WR6 (load)
    IO_6B = 0xCF;
    // WR6 (enable)
    IO_6B = 0x87;

    bank_set_16k(2, bank + 1);
    IO_123B = 0x47;
    // WR6 (disable)
    IO_6B = 0x83;
    // WR6 (load)
    IO_6B = 0xCF;
    // WR6 (enable)
    IO_6B = 0x87;

    bank_set_16k(2, bank + 2);
    IO_123B = 0x87;
    // WR6 (disable)
    IO_6B = 0x83;
    // WR6 (load)
    IO_6B = 0xCF;
    // WR6 (enable)
    IO_6B = 0x87;
}

uint8_t yPos = 16;
uint8_t frameCounter = 10;
uint8_t dir = 0;
uint8_t barindex = 3;

void UpdateCopperBars()
{
    for (uint8_t i = 0; i < 72; i++) {
        gradientBars[barindex+i*3] = CU_WAIT(yPos+i*2,0);
    }

    uint8_t *copperPtr = gradientBars;
    uint16_t copperProgLen = sizeof gradientBars / sizeof *gradientBars;
    for (int j = 0; j < copperProgLen*2; j++) {
        IO_NEXTREG_REG = REG_COPPER_DATA;
        IO_NEXTREG_DAT = copperPtr[j];
    }

    IO_NEXTREG_REG = REG_COPPER_CONTROL_H;
    IO_NEXTREG_DAT = 0xC0;
    IO_NEXTREG_REG = REG_COPPER_CONTROL_L;
    IO_NEXTREG_DAT = 0;

    if (--frameCounter == 0) {
        frameCounter = 10;
        if (dir == 0) {
            yPos++;
            if (yPos > 144) dir = 1;
        } else {
            yPos--;
            if (yPos == 0) dir = 0;
        }
    }
}

void Opening()
{
    DMACopyScreen(BANK_IMAGE_LOADER);
    OpenWindow();

    // Turn off enhanced ULA and set palette to be ULA
    IO_NEXTREG_REG = 0x43;
    IO_NEXTREG_DAT = 0x0;
    
    bm_printf(24, 48, "%fN%fC%fO%fT%f TECH!", RGB2COLOUR(0,7,3), RGB2COLOUR(7,0,2), RGB2COLOUR(7,7,0), RGB2COLOUR(0,7,0), RGB2COLOUR(7,7,0));
    bm_set_text_colour(92, 227);
    bm_printf(24, 64, "WEB: NCOT.UK");
    
    zx_border(16);
    // Choose ULA palette 16
    IO_NEXTREG_REG = REG_PALETTE_INDEX;
    IO_NEXTREG_DAT = 16;

    // Remap it to green
    IO_NEXTREG_REG = REG_PALETTE_VALUE_8;
    IO_NEXTREG_DAT = RGB2COLOUR(0,7,0);

    // Endless loop
    uint8_t *copperPtr = gradientBars;
    uint8_t copperProgLen = sizeof gradientBars / sizeof *gradientBars;
    for (int i = 0; i < copperProgLen*2; i++) {
        IO_NEXTREG_REG = REG_COPPER_DATA;
        IO_NEXTREG_DAT = copperPtr[i];
    }

    IO_NEXTREG_REG = REG_COPPER_CONTROL_H;
    IO_NEXTREG_DAT = 0xC0;
    IO_NEXTREG_REG = REG_COPPER_CONTROL_L;
    IO_NEXTREG_DAT = 0;

    bm_printf(24, 100, "SUBSCRIBE");
    while (1) {
        UpdateCopperBars();
    }
}

uint8_t scrollClouds = 0;
uint8_t scrollMountains = 0;
uint8_t scrollGround = 0;
void UpdateParallaxScroll()
{
    parallaxScroll[3] = CU_MOVE(REG_LAYER_2_OFFSET_X,scrollClouds);
    parallaxScroll[14] = CU_MOVE(REG_LAYER_2_OFFSET_X,scrollMountains);
    parallaxScroll[27] = CU_MOVE(REG_LAYER_2_OFFSET_X,scrollGround);

    uint8_t *copperPtr = parallaxScroll;
    IO_NEXTREG_REG = REG_COPPER_CONTROL_L;
    IO_NEXTREG_DAT = 7;
    IO_NEXTREG_REG = REG_COPPER_DATA;
    IO_NEXTREG_DAT = copperPtr[7];
    IO_NEXTREG_REG = REG_COPPER_CONTROL_L;
    IO_NEXTREG_DAT = 29;
    IO_NEXTREG_REG = REG_COPPER_DATA;
    IO_NEXTREG_DAT = copperPtr[29];
    IO_NEXTREG_REG = REG_COPPER_CONTROL_L;
    IO_NEXTREG_DAT = 55;
    IO_NEXTREG_REG = REG_COPPER_DATA;
    IO_NEXTREG_DAT = copperPtr[55];

 /*   uint8_t *copperPtr = parallaxScroll;
    uint16_t copperProgLen = sizeof parallaxScroll / sizeof *parallaxScroll;
    for (int j = 0; j < copperProgLen*2; j++) {
        IO_NEXTREG_REG = REG_COPPER_DATA;
        IO_NEXTREG_DAT = copperPtr[j];
    }
*/
    IO_NEXTREG_REG = REG_COPPER_CONTROL_H;
    IO_NEXTREG_DAT = 0xC0;
    IO_NEXTREG_REG = REG_COPPER_CONTROL_L;
    IO_NEXTREG_DAT = 0;

    if (--frameCounter == 0) {
        scrollClouds++;
        scrollMountains += 2;
        scrollGround += 3;
        frameCounter = 10;
    }
}

void parallaxScrolling()
{
    zx_border(16);
    // Turn off enhanced ULA and set palette to be ULA
    IO_NEXTREG_REG = 0x43;
    IO_NEXTREG_DAT = 0x0;

    DMACopyScreen(BANK_IMAGE_MAIN);
    uint8_t *copperPtr = parallaxScroll;
    uint8_t copperProgLen = sizeof parallaxScroll / sizeof *parallaxScroll;
    for (int i = 0; i < copperProgLen*2; i++) {
        IO_NEXTREG_REG = REG_COPPER_DATA;
        IO_NEXTREG_DAT = copperPtr[i];
    }

    IO_NEXTREG_REG = REG_COPPER_CONTROL_H;
    IO_NEXTREG_DAT = 0xC0;
    IO_NEXTREG_REG = REG_COPPER_CONTROL_L;
    IO_NEXTREG_DAT = 0;

    while (1) {
        UpdateParallaxScroll();
    }
    
}

int main(void)
{
    intrinsic_di();
    loadImage("loader.nxi", BANK_IMAGE_LOADER);
    loadImage("main1.nxi", BANK_IMAGE_MAIN1);
    loadImage("main2.nxi", BANK_IMAGE_MAIN2);
    loadImage("main.nxi", BANK_IMAGE_MAIN);

    bm_load_font("font1.fon", 2, BANK_FONT1);
    Opening();
   // parallaxScrolling();
    
    while(1);

    return 0;
}

/* C source end */