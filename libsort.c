#include "libsort.h"
#include <stdint.h>
#include <string.h>

#define BYTEP_AT(p, idx, type_size) ((uint8_t *)(p) + (idx) * (type_size))
#define MIN(a, b) ({ \
    typeof(a) __min1__ = (a); \
    typeof(b) __min2__ = (b); \
    (void)(&__min1__ == &__min2__); \
    __min1__ < __min2__ ? __min1__ : __min2__;})

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

static inline
void _pivot(void *array, size_t len, size_t type_size, fn_cmp cmp, fn_swap swap)
{
    size_t mid = len >> 1;
    if (cmp(array, BYTEP_AT(array, mid, type_size)) > 0){
        swap(array, BYTEP_AT(array, mid, type_size));
    }
    if (cmp(array, BYTEP_AT(array, len - 1, type_size)) > 0){
        swap(array, BYTEP_AT(array, len - 1, type_size));
    }
    if (cmp(BYTEP_AT(array, mid, type_size), BYTEP_AT(array, len - 1, type_size)) > 0){
        swap(BYTEP_AT(array, mid, type_size), BYTEP_AT(array, len - 1, type_size));
    }
    swap(BYTEP_AT(array, mid, type_size), BYTEP_AT(array, len - 2, type_size));
}



void quick_sort(void *array, size_t len, size_t type_size,
                fn_cmp cmp, fn_swap swap)
{
    // TODO
}

void int_swap(int array[], int i, int j)
{
    int tmp = array[i];
    array[i] = array[j];
    array[j] = tmp;
}

void reverse(int array[], int low, int high, int len)
{
    if (len <= 0)
        return;
    for (int i = 0; i < len; i++){
        int_swap(array, low+i, high-i);
    }
}

int _dealpivot(int array[], int low, int high)
{
    int mid = low + ((high - low) >> 1);

    if (array[mid] > array[high]){
        int_swap(array, mid, high);
    }
    if (array[low] > array[high]){
        int_swap(array, low, high);
    }
    if (array[mid] > array[low]){
        int_swap(array, mid, low);
    }
    return array[low];
}

void int_qsort(int array[], int low, int high)
{
    if (high - low <= 6){
        // insert
        for (int i = low + 1; i <= high; i++){
            int tmp = array[i];
            int j = i;
            for (; j > low && array[j-1] > tmp; j--){
                array[j] = array[j-1];
            }
            array[j] = tmp;
        }
        return;
    }
    
    /*
     * p|xxxx...xxxxt
     * t >= p must be true
     */
    int pivot = _dealpivot(array, low, high);
    
    int i = low;
    int j = high;
 
    int leftbound = low + 1;
    int rightbound = high;
    for(;;){
        // from left
        for (;;){
            i++;
            if (array[i] < pivot){
                continue;
            }
            if (array[i] > pivot){
                break;
            }
            int k = i;
            int break_flag = 0;
            for (;;){
                i++;
                if (i > high){
                    break_flag = 1;
                    break;
                }
                if (array[i] != pivot){
                    if (array[i] > pivot){
                        break_flag = 1;
                    }
                    break;
                }
            }
            reverse(array, leftbound, i - 1, MIN(i - k, k - leftbound));
            leftbound += i - k;
            if (break_flag){
                break;
            }
        }

        // from right
        while (j > i) {
            if (array[j] > pivot){
                j--;
                continue;
            }
            if (array[j] < pivot){
                break;
            }
            int k = j;
            while(array[--j] == pivot);
            reverse(array, j + 1, rightbound, MIN(k - j, rightbound - k));
            rightbound -= k - j;
        }

        if (i < j){
            int_swap(array, i, j);
        }
        else{
            break;
        }
    }

    reverse(array, low, i - 1, MIN(leftbound - low, i - leftbound));
    reverse(array, j, high, MIN(high - rightbound, rightbound - j + 1));
    
    if (i - leftbound > 1){
        int_qsort(array, low, low + i - leftbound - 1);
    }
    if (rightbound - j > 0){
        int_qsort(array, j + high - rightbound, high);
    } 
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

    if (cmp(j, midp) >= 0){
        memcpy(array, aux, (end + 1) * type_size);
        return;
    }

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

void merge_sort(void *array, size_t len, size_t type_size,
                 fn_cmp cmp, void *aux)
{
    size_t block = 4; // length for insert sort
    size_t i;
    
    int aux_swaped = 0;
    for (i = 0; i < len - block; i += block){
        insert_sort(BYTEP_AT(array, i, type_size), block, type_size, cmp, aux);
    }
    if ((len - i) > 1){
        insert_sort(BYTEP_AT(array, i, type_size), len - i, type_size, cmp, aux);
    }
    
    for (; block < len; block <<= 1){
        u64_swap(&array, &aux);
        aux_swaped ^= 1;
        for (i = 0; i < len - block; i += block << 1){
            _merge(BYTEP_AT(array, i, type_size), BYTEP_AT(aux, i, type_size), block - 1, MIN((block << 1) - 1, len - i - 1), type_size, cmp);
        }
        if (i < len){
            memcpy(BYTEP_AT(array, i, type_size), BYTEP_AT(aux, i, type_size), (len - i) * type_size);
        }
    }

    if (aux_swaped){
        memcpy(aux, array, len * type_size);
    }
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
