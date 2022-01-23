#ifndef __BANKING_H_
#define __BANKING_H_

#include <arch/zxn.h>
#include <z80.h>

uint8_t bank_set_8k(uint8_t mmu, uint8_t page);
uint8_t bank_set_16k(uint8_t mmu, uint8_t page);

#endif