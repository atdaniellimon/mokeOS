#ifndef SYSCALL_H
#define SYSCALL_H
#include "../stdint/types.h"

void syscall_init();
uint32_t syscall_handler(uint32_t num, uint32_t a, uint32_t b, uint32_t c, uint32_t d);

#endif