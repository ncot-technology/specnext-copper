#include "copper.h"
#include <lib/zxn/libchai-layer2.h>
//__sfr __at 0x16 IO_L2HSCROLLCTRL;
//__sfr __at 0x17 IO_L2VSCROLLCTRL;

/*

#define CU_WAIT(ver,hor)  ((unsigned int)((((ver) & 0xff) << 8) + 0x80 + (((hor) & 0x3f) << 1) + (((ver) & 0x100) >> 8)))
#define CU_MOVE(reg,val)  ((unsigned int)((((val) & 0xff) << 8) + ((reg) & 0x7f)))
#define CU_NOP            ((unsigned int)0x0000)
#define CU_STOP           ((unsigned int)0xffff)

*/

static uint16_t copperProgLen;

void InitCopper(uint8_t copperProg[])
{
    uint8_t *copperPtr = copperProg;
    copperProgLen = sizeof copperProg / sizeof *copperProg;
    for (int i = 0; i < copperProgLen*2; i++) {
        IO_NEXTREG_REG = REG_COPPER_DATA;
        IO_NEXTREG_DAT = copperPtr[i];
    }

    IO_NEXTREG_REG = REG_COPPER_CONTROL_H;
    IO_NEXTREG_DAT = 0xC0;
    IO_NEXTREG_REG = REG_COPPER_CONTROL_L;
    IO_NEXTREG_DAT = 0;
}

