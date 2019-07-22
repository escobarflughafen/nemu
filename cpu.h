//
// Created by tachiaoi on 2019-07-12.
//

#ifndef NESIM_CPU_H
#define NESIM_CPU_H

#include <stdint.h>

#define N 7
#define V 6
#define UNDEFINED 5
#define B 4
#define D 3
#define I 2
#define Z 1
#define C 0

#define false 0
#define true 1

enum addressing_mode{
    NONE,       //undefined
    IMP,        //implied
    ACC,        //accumulator
    IMM,        //immediate
    ZER,        //zero page
    ZERX,      //zero page X
    ZERY,      //zero page Y
    REL,        //relative
    ABS,        //absolute
    ABSX,      //absolute indexed by reg X
    ABSY,      //absolute indexed by reg Y
    IND,        //indirect
    INXIND,    //indexed indirect   ONLY USED with REGISTER X
    INDINX     //indirect indexed   ONLY USED with REGISTER Y
};

static const enum addressing_mode address_mode_lookup[256] = {
        IMP, INXIND, NONE, INXIND, ZER, ZER, ZER, ZER, IMP, IMM, ACC, NONE, ABS, ABS, ABS,                  //00 -> 0F
        REL, INDINX, NONE, INDINX, ZERX, ZERX, ZERX, ZERX, IMP, ABSY, NONE, ABSY, ABSX, ABSX, ABSX, ABSX,   //10 -> 1F
        ABS, INXIND, NONE, INXIND, ZER, ZER, ZER, ZER, IMP, IMM, ACC, NONE, ABS, ABS, ABS, ABS,             //20 -> 2F
        REL, INDINX, NONE, INDINX, ZERX, ZERX, ZERX, ZERX, IMP, ABSY, NONE, ABSY, ABSX, ABSX, ABSX, ABSX,   //30 -> 3F
        IMP, INXIND, NONE, INXIND, ZER, ZER, ZER, ZER, IMP, IMM, ACC, NONE, ABS, ABS, ABS, ABS,             //40 -> 4F
        REL, INDINX, NONE, INDINX, ZERX, ZERX, ZERX, ZERX, IMP, ABSY, NONE, ABSY, ABSX, ABSX, ABSX, ABSX,   //50 -> 5F
        IMP, INXIND, NONE, INXIND, ZER, ZER, ZER, ZER, IMP, IMM, ACC, NONE, IND, ABS, ABS, ABS,             //60 -> 6F
        REL, INDINX, NONE, INDINX, ZERX, ZERX, ZERX, ZERX, IMP, ABSY, NONE, ABSY, ABSX, ABSX, ABSX, ABSX,   //70 -> 7F
        IMM, INXIND, NONE, INXIND, ZER, ZER, ZER, ZER, IMP, NONE, IMP, NONE, ABS, ABS, ABS, ABS,            //80 -> 8F
        REL, INDINX, NONE, NONE, ZERX, ZERX, ZERY, ZERY, IMP, ABSY, IMP, NONE, NONE, ABSX, NONE, NONE,      //90 -> 9F
        IMM, INXIND, IMM, INXIND, ZER, ZER, ZER, ZER, IMP, IMM, IMP, NONE, ABS, ABS, ABS, ABS,              //A0 -> AF
        REL, INDINX, NONE, INDINX, ZERX, ZERX, ZERY, ZERY, IMP, ABSY, IMP, NONE, ABSX, ABSX, ABSY, ABSY,    //B0 -> BF
        IMM, INXIND, NONE, INXIND, ZER, ZER, ZER, ZER, IMP, IMM, IMP, NONE, ABS, ABS, ABS, ABS,             //C0 -> CF
        REL, INDINX, NONE, INDINX, ZERX, ZERX, ZERX, ZERX, IMP, ABSY, NONE, ABSY, ABSX, ABSX, ABSX, ABSX,   //D0 -> DF
        IMM, INXIND, NONE, INXIND, ZER, ZER, ZER, ZER, IMP, IMM, NONE, IMM, ABS, ABS, ABS, ABS,             //E0 -> EF
        REL, INDINX, NONE, INDINX, ZERX, ZERX, ZERX, ZERX, IMP, ABSY, NONE, ABSY, ABSX, ABSX, ABSX, ABSX    //F0 -> FF
};

static const int clock_cycle_lookup[256] = {
        7, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,     //00 -> 0F
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,     //10 -> 1F
        6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,     //20 -> 2F
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,     //30 -> 3F
        6, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,     //40 -> 4F
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,     //50 -> 5F
        6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,     //60 -> 6F
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,     //70 -> 7F
        2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 0, 4, 4, 4, 4,     //80 -> 8F
        2, 6, 0, 0, 4, 4, 4, 4, 2, 5, 2, 0, 0, 5, 0, 0,     //90 -> 9F
        2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 0, 4, 4, 4, 4,     //A0 -> AF
        2, 5, 0, 5, 4, 4, 4, 4, 2, 4, 2, 0, 4, 4, 4, 4,     //B0 -> BF
        2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,     //C0 -> CF
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,     //D0 -> DF
        2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,     //E0 -> EF
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7      //F0 -> FF
};

static const int page_breaking_lookup[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //00 -> 0F
        0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,     //10 -> 1F
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //20 -> 2F
        0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,     //30 -> 3F
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,     //40 -> 4F
        0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,     //50 -> 5F
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //60 -> 6F
        0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,     //70 -> 7F
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //80 -> 8F
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //90 -> 9F
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //A0 -> AF
        0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0,     //B0 -> BF
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //C0 -> CF
        0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,     //D0 -> DF
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //E0 -> EF
        0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0      //F0 -> FF
};

struct cpu6502_state {
    uint16_t    PC;        //program counter
    uint16_t    SP;        //stack pointer
    uint8_t     A;          //accumulator register
    uint8_t     X;          //
    uint8_t     Y;          //
    uint8_t     P;          //status register
};

extern struct cpu6502_state cpu_state;

int         init_cpu6502        (struct cpu6502_state *state);

int         update_ZN           (struct cpu6502_state *state, uint8_t variable);
uint8_t     fetch_and_execute   (struct cpu6502_state *state);

void        NMI                 (struct cpu6502_state *state);

uint16_t    get_address         (struct cpu6502_state *state, uint8_t opcode);

uint16_t    address_IMM         (struct cpu6502_state *state);
uint16_t    address_ZER         (struct cpu6502_state *state);
uint16_t    address_ZERX        (struct cpu6502_state *state);
uint16_t    address_ZERY        (struct cpu6502_state *state);
int8_t      address_REL         (struct cpu6502_state *state);
uint16_t    address_ABS         (struct cpu6502_state *state);
uint16_t    address_ABSX        (struct cpu6502_state *state);
uint16_t    address_ABSY        (struct cpu6502_state *state);
uint16_t    address_IND         (struct cpu6502_state *state);
uint16_t    address_INXIND      (struct cpu6502_state *state);
uint16_t    address_INDINX      (struct cpu6502_state *state);

void        ADC                 (struct cpu6502_state *state, uint8_t arg);
void        SBC                 (struct cpu6502_state *state, uint8_t arg);
uint8_t     LSR                 (struct cpu6502_state *state, uint8_t arg);
uint8_t     ASL                 (struct cpu6502_state *state, uint8_t arg);
uint8_t     ROR                 (struct cpu6502_state *state, uint8_t arg);
uint8_t     ROL                 (struct cpu6502_state *state, uint8_t arg);
void        DEC                 (struct cpu6502_state *state, uint16_t arg);
void        INC                 (struct cpu6502_state *state, uint16_t arg);
void        SLO                 (struct cpu6502_state *state, uint16_t arg);
void        RLA                 (struct cpu6502_state *state, uint16_t arg);
void        RRA                 (struct cpu6502_state *state, uint16_t arg);
void        SRE                 (struct cpu6502_state *state, uint16_t arg);
uint8_t     BXX                 (struct cpu6502_state *state, uint16_t address, int flag, int should_be_set);


#endif //NESIM_CPU_H
