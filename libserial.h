#ifndef LIBSERIAL_H
#define LIBSERIAL_H

#include <stddef.h>
#include <stdint.h>

// type for serial
#include "libbitarray.h"

/*
    |--------------capacity-----------------------------|
    |----8 bytes----|---------data---------|            |
    ↑               ↑                      ↑
   head  length   quence                 stack    
 */
typedef struct serial{
    uint8_t *head;
    uint8_t *quence;
    uint8_t *stack;
    size_t capacity;
}Serial;

void serial_init(Serial *sel, uint8_t *head, size_t len);
void serial_free(Serial *sel);
void serial_ensure(Serial *sel, size_t len);

void serial_bitarray(const Bitarray *ba, Serial *sel);
void unserial_bitarray(Bitarray *ba, Serial *sel);

#endif
