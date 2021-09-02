#include "libsort.h"
#include <stdint.h>
#include <string.h>

#define BYTEP_AT(p, idx, type_size) ((uint8_t *)(p) + (idx) * (type_size))

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
    uint8_t *end = BYTEP_AT(array, len - 1, type_size);

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
    uint8_t *end = BYTEP_AT(array, len - 1, type_size);
    
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
void _merge(void *array, void *aux, size_t mid, size_t end, size_t type_size, fn_cmp cmp)
{
    uint8_t *i = aux;
    uint8_t *j = BYTEP_AT(aux, mid + 1, type_size);
    uint8_t *midp = BYTEP_AT(aux, mid, type_size);
    uint8_t *highp = BYTEP_AT(aux, end, type_size);
    uint8_t *k = array;
    uint8_t *kend = BYTEP_AT(array, end, type_size);

    for (; k <= kend; k += type_size){
        if (i > midp){
            memcpy(k, j, kend - k + type_size);
            break;
        }
        if (j > highp){
            memcpy(k, i, kend - k + type_size);
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
void _merge_sort_recur(void *array, void *aux, size_t end, size_t type_size, fn_cmp cmp)
{
    if (end < 4){
        insert_sort(array, end + 1, type_size, cmp, aux);
        return;
    }
    size_t mid = end >> 1;
    _merge_sort_recur(aux, array, mid, type_size, cmp);
    _merge_sort_recur(BYTEP_AT(aux, mid + 1, type_size), BYTEP_AT(array, mid + 1, type_size), end - mid - 1, type_size, cmp);
    if (cmp(BYTEP_AT(aux, mid, type_size), BYTEP_AT(aux, mid + 1, type_size)) <= 0){
        memcpy(array, aux, (end + 1) * type_size);
    }
    else{
        _merge(array, aux, mid, end, type_size, cmp);
    }
}

void merge_sort(void *array, size_t len, size_t type_size,
                fn_cmp cmp, void *aux)
{
    memcpy(aux, array, len * type_size);
    _merge_sort_recur(array, aux, len - 1, type_size, cmp);
}


void shuffle(void *array, size_t len, size_t type_size,
             size_t (*randint)(size_t bound),
             fn_swap swap)
{
    uint8_t *i = array, *j;
    uint8_t *end = BYTEP_AT(array, len - 1, type_size);

    if (!swap){
        swap = swap_check(type_size);
    }

    for (; i < end; i += type_size){
        j = i + randint(len--) * type_size;
        swap(i, j);
    }
}

int sort_check(void *array, size_t len, size_t type_size,
               fn_cmp cmp)
{
    uint8_t *i = array;
    uint8_t *end = BYTEP_AT(array, len - 1, type_size);

    for (; i < end; i += type_size){
        if (cmp(i, i + type_size) > 0){
            return 0;
        }
    }
    return 1;
}
