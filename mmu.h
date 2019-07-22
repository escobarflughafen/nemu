//
// Created by tachiaoi on 2019-07-12.
//

#ifndef NESIM_MMU_H
#define NESIM_MMU_H

#define PPUCTRL     0x2000
#define PPUMASK     0x2001
#define PPUSTATUS   0x2002
#define OAMADDR     0x2003
#define OAMDATA     0x2004
#define PPUSCROLL   0x2005
#define PPUADDR     0x2006
#define PPUDATA     0x2007
#define OAMDMA      0x4014

#include <stdint.h>

struct memory_manager {
    uint8_t     *RAM;
    uint8_t     *VRAM;
    uint8_t     *OAM;

    uint8_t     w;
    uint8_t     t;
    uint8_t     x;
    uint8_t     y;

    uint16_t    VRAM_address;
    uint16_t    internal_buffer;
};

extern struct memory_memory MMU;

void init_MMU();

uint8_t     write_RAM       (uint16_t address, uint8_t value);
uint8_t     read_RAM        (uint16_t address);

void        write_VRAM      (uint16_t address, uint8_t value);
uint8_t     read_VRAM       (uint16_t address);

void        write_SPRRAM    (uint8_t address, uint8_t value);
uint8_t     read_SPRRAM     (uint8_t address);

void        dump_RAM        ();
void        dump_VRAM       ();
void        dump_SPRRAM     ();

#endif //NESIM_MMU_H
