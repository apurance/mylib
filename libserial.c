#include "libserial.h"
#include <stdlib.h>
#include <string.h>

void serial_init(Serial *sel, uint8_t *head, size_t len)
{
    if (head){
        sel->head = head;
        sel->quence = head + 8;
        sel->stack = head + len;
        sel->capacity = len;
        return;
    }
    sel->head = calloc(len + 8, 1);
    *sel->head = 8;
    sel->quence = sel->head + 8;
    sel->stack = sel->quence;
    sel->capacity = len + 8;
}

void serial_free(Serial *sel)
{
    free(sel->head);
    sel->head = NULL;
    sel->quence = NULL;
    sel->stack = NULL;
    sel->capacity = 0;
}

void serial_ensure(Serial *sel, size_t len)
{
    size_t stack_len = *((uint64_t *)sel->head); // sel->stack - sel->head
    if ((sel->capacity - stack_len) < len){
        size_t quence_len = sel->quence - sel->head;
        size_t new_cap = sel->capacity > len ? sel->capacity << 1 : sel->capacity + len;
        uint8_t *new_head = malloc(new_cap);
        memcpy(new_head, sel->head, stack_len);
        free(sel->head);
        sel->head = new_head;
        sel->quence = new_head + quence_len;
        sel->stack = new_head + stack_len;
        sel->capacity = new_cap;
    }
}

void serial_bitarray(const Bitarray *ba, Serial *sel)
{
    serial_ensure(sel, 8+ba->num);
    *((uint64_t *)sel->stack) = bitarray_len(ba);
    sel->stack += 8;
    memcpy(sel->stack, ba->array, ba->num);
    sel->stack += ba->num;
    // head update
    *((uint64_t *)sel->head) += 8 + ba->num;
}

void unserial_bitarray(Bitarray *ba, Serial *sel)
{
    // can reach? serial_reach()
    size_t len = *((uint64_t *)sel->quence);
    sel->quence += 8;

    size_t new_num = len >> 3;
    uint8_t new_bit = (len - 1) & 7;
    if (len & 7)
        new_num++;
    if (ba->capacity < new_num){
        if (ba->capacity)
            free(ba->array);
        ba->array = malloc(new_num);
        ba->capacity = new_num;
    }
    memcpy(ba->array, sel->quence, new_num);
    sel->quence += new_num;

    ba->num = new_num;
    ba->bit = new_bit;
}
