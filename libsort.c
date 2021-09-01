#include "libsort.h"
#include <stdint.h>

static size_t g_type_size;

static inline
void general_swap(void *x, void *y)
{
    uint8_t tmp;
    size_t i;

    for (i = 0; i < g_type_size; i++, x++, y++){
        tmp = *(uint8_t *)x;
        *(uint8_t *)x = *(uint8_t *)y;
        *(uint8_t *)y = tmp;
    }
}

static inline
void u8_swap(void *x, void *y)
{
    uint8_t tmp;
    tmp = *(uint8_t *)x;
    *(uint8_t *)x = *(uint8_t *)y;
    *(uint8_t *)y = tmp;
}

static inline
void u16_swap(void *x, void *y)
{
    uint16_t tmp;
    tmp = *(uint16_t *)x;
    *(uint16_t *)x = *(uint16_t *)y;
    *(uint16_t *)y = tmp;
}

static inline
void u32_swap(void *x, void *y)
{
    uint32_t tmp;
    tmp = *(uint32_t *)x;
    *(uint32_t *)x = *(uint32_t *)y;
    *(uint32_t *)y = tmp;
}

static inline
void u64_swap(void *x, void *y)
{
    uint64_t tmp;
    tmp = *(uint64_t *)x;
    *(uint64_t *)x = *(uint64_t *)y;
    *(uint64_t *)y = tmp; 
}

static inline
fn_swap swap_check(size_t type_size)
{
    switch(type_size){
        case 1:
            return u8_swap;
        case 2:
            return u16_swap;
        case 4:
            return u32_swap;
        case 8:
            return u64_swap;
        default:
            g_type_size = type_size;
            return general_swap;
    }
}

void select_sort(void *array, size_t len, size_t type_size,
                 fn_cmp cmp,
                 fn_swap swap)
{
    uint8_t *i, *j, *k;
    uint8_t *end = array + (len - 1) * type_size;

    if (!swap){
        swap = swap_check(type_size);
    }

    for (i = array; i < end; i += type_size){
        k = i;
        for (j = i + type_size; j <= end; j += type_size){
            if (cmp(k, j) > 0){
                k = j;
            }
        }
        if (k != i){
            swap(k, i);
        }
    }
}

void quick_sort(void *array, size_t len, size_t type_size,
                 fn_cmp cmp,
                 fn_swap swap)
{
    // TODO
}

void shuffle(void *array, size_t len, size_t type_size,
                 size_t (*randint)(size_t bound),
                 fn_swap swap)
{
    uint8_t *i = array, *j;
    uint8_t *end = (uint8_t *)array + (len - 1) * type_size;

    if (!swap){
        swap = swap_check(type_size);
    }

    for (; i < end; i += type_size){
        j = i + randint(len--) * type_size;
        swap(i, j);
    }
}