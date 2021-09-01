#include "libsort.h"
#include <stdint.h>
#include <string.h>

#define TMP_SIZE 128

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
                 fn_cmp cmp, fn_swap swap)
{
    uint8_t *i, *j, *k;
    uint8_t *end = (uint8_t *)array + (len - 1) * type_size;

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
                fn_cmp cmp, fn_swap swap)
{
    // TODO
}

void insert_sort(void *array, size_t len, size_t type_size,
                 fn_cmp cmp, void *tmp)
{
    uint8_t *i, *j;
    uint8_t *end = (uint8_t *)array + (len - 1) * type_size;
    
    for (i = (uint8_t *)array + type_size; i <= end; i += type_size){
        // back search
        for (j = i - type_size; j >= (uint8_t *)array; j -= type_size){
            if (cmp(i, j) >= 0){
                break;
            }
        }
        j += type_size;
        // insert
        if (i > j){
            memcpy(tmp, i, type_size);
            memmove(j + type_size, j, i - j);
            memcpy(j, tmp, type_size);
        }
    }
}

static
void _merge(void *array, void *aux, size_t low, size_t mid, size_t high, size_t type_size, fn_cmp cmp)
{
    uint8_t *i = (uint8_t *)aux + low * type_size;
    uint8_t *j = (uint8_t *)aux + (mid + 1) * type_size;
    uint8_t *midp = (uint8_t *)aux + mid * type_size;
    uint8_t *highp = (uint8_t *)aux + high * type_size;
    uint8_t *k = (uint8_t *)array + low * type_size;
    uint8_t *end = (uint8_t *)array + high * type_size;

    for (; k <= end; k += type_size){
        if (i > midp){
            memcpy(k, j, end - k + type_size);
            break;
        }
        if (j > highp){
            memcpy(k, i, end - k + type_size);
            break;
        }
        if (cmp(i, j) > 0){
            memcpy(k, j, type_size);
            j += type_size;
        }
        else{
            memcpy(k, i, type_size);
            i += type_size;
        }
    }
}

static
void _merge_sort_recur(void *array, void *aux, size_t low, size_t high,size_t type_size, fn_cmp cmp)
{
    if (low >= high)
        return;
    size_t mid = low + ((high - low) >> 1);
    _merge_sort_recur(aux, array, low, mid, type_size, cmp);
    _merge_sort_recur(aux, array, mid + 1, high, type_size, cmp);
    if (cmp((uint8_t *)aux + mid * type_size, (uint8_t *)aux + (mid + 1) * type_size) <= 0){
        memcpy((uint8_t *)array + low * type_size, (uint8_t *)aux + low * type_size, (high - low + 1) * type_size);
    }
    else{
        _merge(array, aux, low, mid, high, type_size, cmp);
    }
}

void merge_sort(void *array, size_t len, size_t type_size,
                fn_cmp cmp, void *aux)
{
    memcpy(aux, array, len * type_size);
    _merge_sort_recur(array, aux, 0, len - 1, type_size, cmp);
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