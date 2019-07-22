//
// Created by tachiaoi on 2019-07-18.
#include "cpu.h"
#include "mmu.h"

uint16_t address_IMM(struct cpu6502_state *state) {
    return state->PC++;
}

uint16_t address_ZER(struct cpu6502_state *state) {
    return read_RAM(state->PC++);
}

uint16_t address_ZERX(struct cpu6502_state *state) {
    return (read_RAM(state->PC++ )+ state->X) & 0xFF;
}

uint16_t address_ZERY(struct cpu6502_state *state) {
    return (read_RAM(state->PC++) + state->Y) & 0xFF;

}

int8_t address_REL(struct cpu6502_state *state) {
    return read_RAM(state->PC++);
}

uint16_t address_ABS(struct cpu6502_state *state) {
    uint8_t low = read_RAM(state->PC++);
    uint8_t high = read_RAM(state->PC++);
    return  (high << 8) | low;
}

uint16_t address_ABSX(struct cpu6502_state *state) {
    uint8_t low = read_RAM(state->PC++);
    uint8_t high = read_RAM(state->PC++);
    return ((high<< 8) | low) + state->X;
}

uint16_t address_ABSY(struct cpu6502_state *state) {
    uint8_t low = read_RAM(state->PC++);
    uint8_t high = read_RAM(state->PC++);
    return ((high<< 8) | low) + state->Y;
}

uint16_t address_IND(struct cpu6502_state *state) {
    uint8_t low = read_RAM(state->PC++);
    uint8_t high = read_RAM(state->PC++);
    uint16_t target = (high << 8) | low;

    uint8_t t_low = read_RAM((high << 8) | low);
    uint8_t t_high = read_RAM(((high << 8) | low) + 1);
    if (((((high << 8) | low) + 1) & 0xFF) == 0) {
        t_high = read_RAM((((high << 8) | low) + 1) - 0x100);
    }

    return (t_high << 8) | t_low;
}
uint16_t address_INXIND(struct cpu6502_state *state) {
    uint8_t address = read_RAM(state->PC++);

    uint8_t low = read_RAM((address + state->X) & 0xFF);
    uint8_t high = read_RAM((address + state->X + 1) & 0xFF);

    return (high << 8) | low;
}

uint16_t address_INDINX(struct cpu6502_state *state) {
    uint32_t address = read_RAM(state->PC++);

    uint32_t low = read_RAM(address++);
    uint32_t high = read_RAM(address & 0xFF);

    return (((high << 8) | low) + state->Y);
}

uint16_t get_address (struct cpu6502_state *state, uint8_t opcode) {
    enum addressing_mode mode = address_mode_lookup[opcode];
    switch (mode) {
        case IMM:
            return address_IMM(state);
        case ZER:
            return address_ZER(state);
        case ZERX:
            return address_ZERX(state);
        case ZERY:
            return address_ZERY(state);
        case REL:
            return state->PC + address_REL(state);
        case ABS:
            return address_ABS(state);
        case ABSX:
            return address_ABSX(state);
        case ABSY:
            return address_ABSY(state);
        case IND:
            return address_IND(state);
        case INXIND:
            return address_INXIND(state);
        case INDINX:
            return address_INDINX(state);
        case NONE:
        case IMP:
        case ACC:
            return 0;
    }
    return 0;
}

void ADC(struct cpu6502_state *state, uint8_t arg) {

}

void SBC(struct cpu6502_state *state, uint8_t arg) {}

uint8_t LSR(struct cpu6502_state *state, uint8_t arg) {}

uint8_t ASL(struct cpu6502_state *state, uint8_t arg) {}

uint8_t ROR(struct cpu6502_state *state, uint8_t arg) {}

uint8_t ROL(struct cpu6502_state *state, uint8_t arg) {}

void DEC(struct cpu6502_state *state, uint16_t arg) {}

void INC(struct cpu6502_state *state, uint16_t arg) {}

void SLO(struct cpu6502_state *state, uint16_t arg) {}

void RLA(struct cpu6502_state *state, uint16_t arg) {}

void RRA(struct cpu6502_state *state, uint16_t arg) {}

void SRE(struct cpu6502_state *state, uint16_t arg) {}

uint8_t BXX(struct cpu6502_state *state, uint16_t address, int flag, int should_be_set) {}
//

