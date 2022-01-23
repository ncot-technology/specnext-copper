/* This is just a test, to give the compiler something to do */
/* Obviously delete this file! */

#include <arch/zxn.h>
#include <z80.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <intrinsic.h>
#include <arch/zxn/esxdos.h>
#include <arch/zxn/copper.h>

#include "banking.h"

#define RGB2COLOUR(r,g,b) (uint8_t)(((r & 0x7) << 5) | ((g & 0x7) << 2) | (b & 0x3))
#define WAIT_FOR_SCANLINE(line)         while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) == line); \
                                        while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) != line)

#define BANK_IMAGE_MAIN 16

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

uint8_t scrollClouds = 0;
uint8_t scrollMountains = 0;
uint8_t scrollGround = 0;
uint8_t frameCounter = 5;

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

    IO_NEXTREG_REG = REG_COPPER_CONTROL_H;
    IO_NEXTREG_DAT = 0xC0;
    IO_NEXTREG_REG = REG_COPPER_CONTROL_L;
    IO_NEXTREG_DAT = 0;

    if (--frameCounter == 0) {
        scrollClouds++;
        scrollMountains += 2;
        scrollGround += 3;
        frameCounter = 5;
    }
}

int main(void)
{
    intrinsic_di();
    
    loadImage("main.nxi", BANK_IMAGE_MAIN);

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
        WAIT_FOR_SCANLINE(192);
    }
}

/* C source end */