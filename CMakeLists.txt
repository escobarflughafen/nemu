cmake_minimum_required(VERSION 3.13)
project(nesim C)

set(CMAKE_C_STANDARD 99)

add_executable(nesim main.c cpu.c cpu.h mmu.c mmu.h ppu.c ppu.h controller.c controller.h cpu_instructions.c)