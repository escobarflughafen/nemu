//
// Created by tachiaoi on 2019-07-12.
//

#ifndef NESIM_CONTROLLER_H
#define NESIM_CONTROLLER_H
#define SDL_Event int
#include <stdint.h>

struct controller {
    uint8_t pointer;
    uint8_t button_status[8];
    uint8_t previous_write;
};

extern struct controller NES_controller;

int         init_controller     (struct controller *controller);
void        handle_input        (struct controller *controller, SDL_Event *e);
void        write_controller    (struct controller *controller, uint8_t data);
uint8_t     get_next_button     (struct controller *controller);



#endif //NESIM_CONTROLLER_H
