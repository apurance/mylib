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
void u32_swap(void *x, void *y)
{
    uint32_t tmp;
    tmp = *(uint32_t *)x;
    *(uint32_t *)x = *(uint32_t *)y;
    *(uint32_t *)y = tmp;
}

void select_sort(void *array, size_t len, size_t type_size,
                 int (*fn_cmp)(void *x, void *y),
                 void (*fn_swap)(void *x, void *y))
{
    uint8_t *i, *j, *k;
    uint8_t *end = array + (len - 1) * type_size;

    if (!fn_swap){
        switch (type_size)
        {
        case 4:
            fn_swap = u32_swap;
            break;
        
        default:
            g_type_size = type_size;
            fn_swap = general_swap;
            break;
        }
    }

    for (i = array; i < end; i += type_size){
        k = i;
        for (j = i + type_size; j <= end; j += type_size){
            if (fn_cmp(k, j) > 0){
                k = j;
            }
        }
        if (k != i){
            fn_swap(k, i);
        }
    }
}