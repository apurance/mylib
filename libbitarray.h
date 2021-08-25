#ifndef LIBBITARRAY_H
#define LIBBITARRAY_H

#include <stdint.h>
#include <stddef.h>

typedef struct bitarray {
    uint8_t *array;
    size_t num;
    uint8_t bit; //0~7
    size_t capacity;
}Bitarray;

static inline size_t bitarray_len(const Bitarray *ba)
{
    return ((ba->num - 1) << 3) + ba->bit + 1;
}

static inline uint8_t bitarray_get(const Bitarray *ba, size_t idx)
{
    return (ba->array[idx >> 3] >> (idx & 7)) & 1;
}

static inline void bitarray_set(const Bitarray *ba, size_t idx, uint8_t b)
{
    uint8_t mask_set = 1 << (idx & 7);

    if (b)
        ba->array[idx >> 3] |= mask_set;
    else
        ba->array[idx >> 3] &= ~mask_set;
}

static inline void bitarray_set_1(const Bitarray *ba, size_t idx)
{
    ba->array[idx >> 3] |= 1 << (idx & 7);
}

static inline void bitarray_set_0(const Bitarray *ba, size_t idx)
{
    ba->array[idx >> 3] &= ~(1 << (idx & 7));
}

static inline void bitarray_switch(const Bitarray *ba, size_t idx)
{
    ba->array[idx >> 3] ^= 1 << (idx & 7);
}

// API
void bitarray_init(Bitarray *ba, size_t len);
void bitarray_init_memory(Bitarray *ba, const uint8_t *mem, size_t len);
void bitarray_free(Bitarray *ba);
uint8_t* bitarray_move(Bitarray *ba);
uint8_t bitarray_equal(Bitarray *x, Bitarray *y);
void bitarray_endian_convert(const Bitarray *ba);
void bitarray_reserve(Bitarray *ba, size_t capacity);
void bitarray_resize(Bitarray *ba, size_t size);
void bitarray_copy(Bitarray *dba, const Bitarray *sba);
void bitarray_set_zeros(Bitarray *ba, size_t len);
void bitarray_set_ones(Bitarray *ba, size_t len);
void bitarray_set_rands(Bitarray *ba, size_t len, unsigned int seed);
void bitarray_set_str(Bitarray *ba, const char *str, size_t len);
size_t bitarray_count(const Bitarray *ba, size_t start, size_t end);
void bitarray_push(Bitarray *ba, uint8_t b);
void bitarray_repeat_push(Bitarray *ba, uint8_t b, size_t repetation);
void bitarray_push_ba(Bitarray *ba, const Bitarray *pba);
void bitarray_sub_set(Bitarray *ba, uint8_t b, size_t start, size_t end);
size_t bitarray_get_str(const Bitarray *ba, char *str);

#endif
