//
// Created by tachiaoi on 2019-07-12.
//

#ifndef NESIM_PPU_H
#define NESIM_PPU_H

#define SDL_Surface int

#include <stdint.h>

static const uint32_t pixel_width = 2;
static const uint32_t pixel_height = 2;

static const uint32_t ppu_colors[64] = {
        0x757575, 0x271b8f, 0x0000ab, 0x47009f, 0x8f0077, 0xab0013, 0xa70000, 0x7f0b00, 0x432f00, 0x004700, 0x005100, 0x003f17, 0x1b3f5f, 0x000000, 0x000000, 0x000000,
        0xbcbcbc, 0x0073ef, 0x233bef, 0x8300f3, 0xbf00bf, 0xe7005b, 0xdb2b00, 0xcb4f0f, 0x8b7300, 0x009700, 0x00ab00, 0x00933b, 0x00838b, 0x000000, 0x000000, 0x000000,
        0xffffff, 0x3fbfff, 0x5f97ff, 0xa78bfd, 0xf77bff, 0xff77b7, 0xff7763, 0xff9b3b, 0xf3bf3f, 0x83d313, 0x4fdf4b, 0x58f898, 0x00ebdb, 0x000000, 0x000000, 0x000000,
        0xffffff, 0xabe7ff, 0xc7d7ff, 0xd7cbff, 0xffc7ff, 0xffc7db, 0xffbfb3, 0xffdbab, 0xffe7a3, 0xe3ffa3, 0xabf3bf, 0xb3ffcf, 0x9ffff3, 0x000000, 0x000000, 0x000000
};

struct PPU_sprite {
    uint8_t     shifts_remaining;
    uint8_t     sprite_index;
    uint8_t     byte2;

    uint8_t     shift_register_0;
    uint8_t     shift_register_1;
    uint8_t     attribute;
    uint8_t     x;
};

struct ppu2C02_state {
    uint8_t     odd_frame;
    uint16_t    scanline;
    uint16_t    dot;

    uint8_t     nmi_occurred;

    uint8_t     nmi_output;
    uint8_t     sprite_zero_hit;

    uint16_t    nametable_base;

    uint16_t    bitmap_shift_0_latch;
    uint16_t    bitmap_shift_1_latch;
    uint16_t    bitmap_shift_0;
    uint16_t    bitmap_shift_1;

    uint16_t    AT_shift_0_latch;
    uint16_t    AT_shift_1_latch;
    uint16_t    AT_shift_0;
    uint16_t    AT_shift_1;

    uint8_t     num_sprites;
    struct      PPU_sprite sprites[8];
};

extern struct ppu2C02_state ppu_state;


int         init_ppu2C02                (struct ppu2C02_state *state);
uint8_t     ppu_cycle                   (struct ppu2C02_state *state, SDL_Surface *screen);
void        handle_visible_scanline     (struct ppu2C02_state *state);
void        horinc                      ();
void        verine                      ();

uint8_t     rendering_enabled           ();

void        set_pixel_color             (SDL_Surface *screen, int x, int y, uint32_t color);
void        render_pixel                (SDL_Surface *screen, struct ppu2C02_state *state);
void        update_ppu_rendering_data   (struct ppu2C02_state *state);

uint16_t    get_sprite_palette_base     (uint8_t attribute_value);
uint16_t    get_background_palette_base (uint16_t attribute_value);

void        fetch_attribute             (struct ppu2C02_state *state);
uint8_t     get_attribute_table_value   (uint16_t attribute_address, uint8_t x, uint8_t y);

void        load_scanline_sprites       (struct ppu2C02_state *state);
int         get_active_sprite_index     (struct ppu2C02_state *state);

#endif //NESIM_PPU_H
