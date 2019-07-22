//
// Created by tachiaoi on 2019-07-12.
//

#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "mmu.h"
#include "ppu.h"


struct cpu6502_state CPU_state;

int init_cpu6502(struct cpu6502_state *state) {
    state->PC = 0xc0000;
    state->SP = 0xfd;
    state->X = 0;
    state->Y = 0;
    state->P = 0x24;
    state->A = 0;

    uint8_t high = read_RAM(0xfffd);
    uint8_t low = read_RAM(0xfffc);
    state->PC = (high << 8) | low;

    return 1;
}

int update_ZN(struct cpu6502_state *state, uint8_t variable) {
    state->P &= ~(1 << Z);

    if (variable == 0) {
        state->P |= (1 << Z);
    }

    state->P &= ~(1 << N);

    if (variable & (1 << 7)) {
        state->P |= (1 << N);
    }
}

int update_C_compare(struct cpu6502_state *state, int var1, int var2) {
    state->P &= ~(1 << C);
    if (var1 >= var2) {
        state->P |= (1 << C);
    }
}

uint8_t is_page_breaking(struct cpu6502_state *state, uint8_t opcode) {
    uint32_t address, low, high;
    enum addressing_mode mode = address_mode_lookup[opcode];

    switch (mode) {
        case ABSX:
            low = read_RAM(state->PC);
            high = read_RAM(state->PC + 1);

            //
            if (((((high << 8) | low) + state->X) & 0xFF00) != (((high << 8) | low) & 0xFF00)) {
                return 1;
            }
            return 0;
        case ABSY:
            low = read_RAM(state->PC);
            high = read_RAM(state->PC + 1);
            if (((((high << 8) | low) + state->X) & 0xFF00) != (((high << 8) | low) & 0xFF00)) {
                return 1;
            }
            return 0;
        case INDINX:
            address = read_RAM(state->PC);
            low = read_RAM(address++);
            high = read_RAM(address & 0xFF);

            if (((((high << 8) | low) + state->Y) & 0xFF00) != (((high << 8) | low) & 0xFF00)) {
                return 1;
            }
            return 0;
        default:
            return 0;
    }
}

void push_stack(struct cpu6502_state *state, int value) {
    write_RAM(0x100 + state->SP, value);

    state->SP -= 1;
    state->SP &= 0xFF;
}

uint8_t pop_stack(struct cpu6502_state *state) {
    state->SP += 1;
    state->SP &= 0xFF;

    return read_RAM((0x100 + state->SP) & 0x1FF);
}


void NMI(struct cpu6502_state *state) {
    uint8_t low = read_RAM(0xFFFA);
    uint8_t high = read_RAM(0xFFFB);
    uint16_t address = (high << 8) | low;

    push_stack(state, state->PC >> 8);
    push_stack(state, state->PC & 0xFF);
    push_stack(state, state->P);

    state->PC = address;
}

/*
 * Instructions by Name:                        OPCODE                   STATUS
 * ADC  -   add with carry
 *
 * AND  -   and (with accumulator)
 * ASL  -   arthimetic shift left
 * BCC  -   branch on carry clear
 * BCS  -   branch on carry set
 * BEQ  -   branch on equal
 * BIT  -   bit test
 * BMI  -   branch on minus (negative set)
 * BNE  -   branch on not equal (zero clear)
 * BPL  -   branch on plus (negative clear)
 * BRK  -   break / interrupt
 * BVC  -   branch on overflow clear
 * BVS  -   branch on overflow set
 * CLC  -   clear carry
 * CLD  -   clear decimal
 * CLI  -   clear interrupt disable
 * CLV  -   clear overflow
 * CMP  -   comapre (with accumulator)
 * CPX  -   compare with X
 * CPY  -   compare with Y
 * DEC  -   decrement
 * DEX  -   decrement X
 * DEY  -   decrement Y
 * EOR  -   exclusive or (with accumulator)
 * INC  -   increment
 * INX  -   increment x
 * INY  -   increment Y
 * JMP  -   jump
 * JSR  -   jump subroutine
 * LDA  -   load accumulator
 * LDX  -   load X
 * LDY  -   load Y
 * LSR  -   logical shift right
 * NOP  -   no operation
 * ORA  -   or with accumulator
 * PHA  -   push accumulator
 * PHP  -   push processor status (SR)
 * PLA  -   pull accumulator
 * PLP  -   pull processor status
 * ROL  -   rotate left
 * ROR  -   rotate right
 * RTI  -   return from interrupt
 * RTS  -   return from subroutine
 * SBC  -   subtract with carry
 * SEC  -   set carry
 * SED  -   set decimal
 * SEI  -   set interrupt disable
 * STA  -   store accumulator
 * STX  -   store X
 * STY  -   store Y
 * TAX  -   transfer accumulator to X
 * TAY  -   transfer accumulator to Y
 * TSX  -   transfer stack pointer to x
 * TXA  -   transfer X to accumulator
 * TXS  -   transfer X to stack pointer
 * TYA  -   transfer Y to accumulator
 */
uint8_t fetch_and_excute(struct cpu6502_state *state) {
    int old_PC = state->PC, old_A = state->A, old_X = state->X, old_Y = state->Y,
            old_P = state->P, old_SP = state->SP;

    char c1[3] = "  ";
    char c2[3] = "  ";
    char c3[3] = "  ";
    char instruction[4] = "   ";
    char after[27] = "                          ";

    uint8_t opcode = read_RAM(state->PC++);
    uint8_t clock_cycles = clock_cycle_lookup[opcode];

    if (is_page_breaking(state, opcode)) {
        uint8_t add = page_breaking_lookup[opcode];
        clock_cycles += add;
    }
    uint16_t address = get_address(state, opcode);

    switch (opcode) {
        case 0xA0:
        case 0xA4:
        case 0xB4:
        case 0xAC:
        case 0xBC:
            state->Y = read_RAM(address);
            update_ZN(state, state->Y);
            sprintf(instruction, "LDY");
            break;

        case 0x84:
        case 0x8C:
        case 0x94:
            clock_cycles += write_RAM(address, state->Y);
            sprintf(instruction, "STY");
            break;

        case 0x86:
        case 0x8E:
        case 0x96:
            clock_cycles += write_RAM(address, state->X);
            sprintf(instruction, "STX");
            break;

        case 0x8A:
            state->A = state->X;
            update_ZN(state, state->A);
            sprintf(instruction, "TXA");
            break;

        case 0x98:
            state->A = state->Y;
            update_ZN(state, state->A);
            sprintf(instruction, "TYA");
            break;

        case 0x9A:
            state->SP = state->X;
            sprintf(instruction, "TXS");
            break;

        case 0xA8:
            state->Y = state->A;
            update_ZN(state, state->Y);
            sprintf(instruction, "TAY");
            break;

        case 0xA9:
        case 0xA5:
        case 0xB5:
        case 0xAD:
        case 0xBD:
        case 0xB9:
        case 0xA1:
        case 0xB1:
            state->A = read_RAM(address);
            update_ZN(state, state->A);
            sprintf(instruction, "LDA");
            break;

        case 0xAA:
            state->X = state->A;
            update_ZN(state, state->X);
            sprintf(instruction, "TAX");
            break;

        case 0xBA:
            state->X = state->SP;
            update_ZN(state, state->X);
            sprintf(instruction, "TSX");
            break;

        case 0x24:
        case 0x2c: {
            int in_memory = read_RAM(address);
            update_ZN(state, in_memory & state->A);
            state->P &= ~(1 << V);
            if (in_memory & (1 << 6))
                state->P |= (1 << V);
            state->P &= ~(1 << V);
            if (in_memory & (1 << 7))
                state->P |= (1 << N);
            sprintf(instruction, "BIT");
            break;
        }
        case 0xC8:
            state->Y += 1;
            update_ZN(state, state->Y);
            sprintf(instruction, "INY");
            break;

        case 0x88:
            state->Y -= 1;
            update_ZN(state, state->Y);
            sprintf(instruction, "DEY");
            break;

        case 0xE8:
            state->X += 1;
            update_ZN(state, state->X);
            sprintf(instruction, "INX");
            break;

        case 0xCA:
            state->X -= 1;
            update_ZN(state, state->X);
            sprintf(instruction, "DEX");
            break;

        case 0x29:
        case 0x25:
        case 0x35:
        case 0x2D:
        case 0x3D:
        case 0x39:
        case 0x21:
        case 0x31:
            state->A &= read_RAM(address);
            update_ZN(state, state->A);
            sprintf(instruction, "AND");
            break;

        case 0xC9:
        case 0xC5:
        case 0xD5:
        case 0xCD:
        case 0xDD:
        case 0xD9:
        case 0xC1:
        case 0xD1: {
            int comparator = read_RAM(address);
            update_C_compare(state, state->A, comparator);
            update_ZN(state, state->A - comparator);
            sprintf(instruction, "CMP");
            break;
        }

        case 0x09:
        case 0x05:
        case 0x15:
        case 0x0D:
        case 0x1D:
        case 0x19:
        case 0x01:
        case 0x11:
            state->A |= read_RAM(address);
            update_ZN(state, state->A);
            sprintf(instruction, "ORA");
            break;

        case 0x49:
        case 0x45:
        case 0x55:
        case 0x4D:
        case 0x5D:
        case 0x59:
        case 0x41:
        case 0x51:
            state->A ^= read_RAM(address);
            update_ZN(state, state->A);
            sprintf(instruction, "EOR");
            break;

        case 0x69:
        case 0x65:
        case 0x75:
        case 0x6D:
        case 0x7D:
        case 0x79:
        case 0x61:
        case 0x71:
            ADC(state, read_RAM(address));
            sprintf(instruction, "ADC");
            break;

        case 0xEB:
        case 0xE9:
        case 0xE5:
        case 0xF5:
        case 0xED:
        case 0xFD:
        case 0xF9:
        case 0xE1:
        case 0xF1:
            SBC(state, read_RAM(address));
            sprintf(instruction, "SBC");
            break;

        case 0xC0:
        case 0xC4:
        case 0xCC: {
            int comparator = read_RAM(address);
            update_C_compare(state, state->Y, comparator);
            update_ZN(state, state->Y - comparator);
            sprintf(instruction, "CPY");
            break;
        }

        case 0xE0:
        case 0xE4:
        case 0xEC: {
            int comparator = read_RAM(address);
            update_C_compare(state, state->X, comparator);
            update_ZN(state, state->X -comparator);
            sprintf(instruction, "CPX");
            break;
        }

        case 0x08:
            push_stack(state, state->P | (1 << UNDEFINED) | (1 << B));
            sprintf(instruction, "PHP");
            break;

        case 0x28:
            state->P = (pop_stack(state) & ~(1<<B) | (1<<UNDEFINED));
            sprintf(instruction, "PLP");
            break;

        case 0x48:
            push_stack(state, state->A);
            sprintf(instruction, "PHA");
            break;

        case 0x68:
            state->A = pop_stack(state);
            update_ZN(state, state->A);
            sprintf(instruction, "PLA");
            break;

        case 0x78:
            state->P |= (1<<I);
            sprintf(instruction, "SEI");
            break;

        case 0x58:
            state->P &= ~(1<<I);
            sprintf(instruction, "CLI");
            break;

        case 0xF8:
            state->P |= (1<<D);
            sprintf(instruction, "SED");
            break;

        case 0xD8:
            state->P &= ~(1<<D);
            sprintf(instruction, "CLD");
            break;

        case 0x38:
            state->P |= (1<<C);
            sprintf(instruction, "SEC");
            break;

        case 0x18:
            state->P &= ~(1<<C);
            sprintf(instruction, "CLC");
            break;

        case 0xB8:
            state->P &= ~(1<<V);
            sprintf(instruction, "CLV");
            break;

        case 0x0A:
            state->A = ASL(state, state->A);
            sprintf(instruction, "ASL");
            break;

        case 0x1E:
        case 0x0E:
        case 0x16:
        case 0x06:
            clock_cycles += write_RAM(address, ASL(state, read_RAM(address)));
            sprintf(instruction, "ASL");
            break;

        case 0x4A:
            state->A = LSR(state, state->A);
            sprintf(instruction, "LSR");
            break;

        case 0x46:
        case 0x56:
        case 0x4E:
        case 0x5E:
            clock_cycles += write_RAM(address, LSR(state, read_RAM(address)));
            sprintf(instruction, "LSR");
            break;

        case 0x6A:
            state->A = ROR(state, state->A);
            sprintf(instruction, "ROR");
            break;

        case 0x66:
        case 0x76:
        case 0x6E:
        case 0x7E:
            clock_cycles += write_RAM(address, ROR(state, read_RAM(address)));
            sprintf(instruction, "ROR");
            break;

        case 0x2A:
            state->A = ROL(state, state->A);
            sprintf(instruction, "ROL");
            break;

        case 0x26:
        case 0x2E:
        case 0x3E:
        case 0x36:
            clock_cycles += write_RAM(address, ROL(state, read_RAM(address)));
            sprintf(instruction, "ROL");
            break;

        case 0x85:
        case 0x95:
        case 0x8D:
        case 0x9D:
        case 0x99:
        case 0x81:
        case 0x91:
            clock_cycles += write_RAM(address, state->A);
            sprintf(instruction, "STA");
            break;

        case 0xE6:
        case 0xF6:
        case 0xEE:
        case 0xFE:
            INC(state, address);
            sprintf(instruction, "INC");
            break;

        case 0xC6:
        case 0xD6:
        case 0xCE:
        case 0xDE:
            DEC(state, address);
            sprintf(instruction, "DEC");
            break;

        case 0xD0:
            BXX(state, address, Z, false);
            sprintf(instruction, "BNE");
            break;

        case 0xB0:
            BXX(state, address, C, true);
            sprintf(instruction, "BCS");
            break;

        case 0xF0:
            BXX(state, address, Z, true);
            sprintf(instruction, "BEQ");
            break;

        case 0x30:
            BXX(state, address, N, true);
            sprintf(instruction, "BMI");
            break;

        case 0x90:
            BXX(state, address, C, false);
            sprintf(instruction, "BCC");
            break;

        case 0x70:
            BXX(state, address, V, true);
            sprintf(instruction, "BVS");
            break;

        case 0x50:
            BXX(state, address, V, false);
            sprintf(instruction, "BVC");
            break;

        case 0x10:
            BXX(state, address, N, false);
            sprintf(instruction, "BPL");
            break;

        case 0x20:
            push_stack(state, (state->PC -1 & 0xFF00)>>8);
            push_stack(state, state->PC -1 & 0xFF);

            state -> PC = address;
            sprintf(instruction, "JSR");
            break;

        case 0x4C:
            state->PC = address;
            sprintf(instruction, "JMP");
            break;

        case 0x6C:
            state->PC = address;
            sprintf(instruction, "JMP");
            break;

        case 0x60: {
            uint8_t low = pop_stack(state);
            uint8_t high = pop_stack(state);
            state->PC = ((high) << 8) | low) +1;
            sprintf(instruction, "RTS");
            break;
        }

        case 0x40: {
            state->P = pop_stack(state) & ~(1 << B) | (1 << UNDEFINED);
            int low = pop_stack(state);
            int high = pop_stack(state);
            state->PC = ((high << 8) | low);
            sprintf(instruction, "RTI");
            break;
        }

        case 0x00: {
            push_stack(state, (state->PC+1 & 0xFF00) >> 8);
            push_stack(state, state->PC+1 & 0xFF);
            push_stack(state, state->P | (1 << UNDEFINED) | (1<<B));
            uint8_t low = read_RAM(0xFFFE);
            uint8_t high = read_RAM(0xFFFF);
            state->PC = (high << 8) | low;
            state->P |= (1<<B);
            sprintf(instruction, "BRK");
            break;
        }

        case 0xA7:
        case 0xB7:
        case 0xAF:
        case 0xBF:
        case 0xA3:
        case 0xB3:
            state->A = read_RAM(address);
            state->X = state->A;
            update_ZN(state, state->X);
            sprintf(instruction, "LAX");
            break;

        case 0x87:
        case 0x97:
        case 0x8F:
        case 0x83:
            clock_cycles += write_RAM(address, state->A & state->X);
            sprintf(instruction, "SAX");
            break;

        case 0xC7:
        case 0xD7:
        case 0xCF:
        case 0xDF:
        case 0xDB:
        case 0xC3:
        case 0xD3: {
            DEC(state, address);
            uint8_t comparator = read_RAM(address);
            update_C_compare(state, state->A, comparator);
            update_ZN(state, state->A - comparator);
            sprintf(instruction, "DCP");
            break;
        }

        case 0xE7:
        case 0xF7:
        case 0xEF:
        case 0xFF:
        case 0xFB:
        case 0xE3:
        case 0xF3:
            INC(state, address);
            SBC(state, read_RAM(address));
            sprintf(instruction, "ISB");
            break;

        case 0x07:
        case 0x17:
        case 0x0F:
        case 0x1F:
        case 0x1B:
        case 0x03:
        case 0x13:
            SLO(state, address);
            sprintf(instruction, "SLO");
            break;

        case 0x27:
        case 0x37:
        case 0x2F:
        case 0x3F:
        case 0x3B:
        case 0x23:
        case 0x33:
            RLA(state, address);
            sprintf(instruction, "RLA");
            break;

        case 0x47:
        case 0x57:
        case 0x4F:
        case 0x5F:
        case 0x43:
        case 0x53:
        case 0x5B:
            SRE(state, address);
            sprintf(instruction, "SRE");
            break;

        case 0x77:
        case 0x6F:
        case 0x7F:
        case 0x7B:
        case 0x67:
        case 0x63:
        case 0x73:
            RRA(state, address);
            sprintf(instruction, "RRA");
            break;

        case 0x04:
        case 0x44:
        case 0x64:
        case 0x80:
        case 0x0C:
        case 0x14:
        case 0x34:
        case 0x54:
        case 0x74:
        case 0xD4:
        case 0xF4:
        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xFC:
        case 0x1A:
        case 0x3A:
        case 0x5A:
        case 0x7A:
        case 0xDA:
        case 0xFA:
        case 0xEA:
            sprintf(instruction, "NOP");
            break;

        default:
            break;

    }

    return clock_cycles;
}