#include "libarray.h"
#include <stdlib.h>
#include <string.h>

void array_init(Array *a, size_t len, size_t type_size)
{
    a->type_size = type_size;
    a->array = NULL;
    if (len)
        a->array = malloc(type_size * len);
    
    if (a->array){
        a->len = len;
        a->capacity = len;
    }
    else{
        a->len = 0;
        a->capacity = 0;
    }
}

void array_free(Array *a)
{
    if (a->array)
        free(a->array);
    a->array = NULL;
    a->len = 0;
    a->capacity = 0;
    a->type_size = 0;
}

int array_equal(const Array *x, const Array *y, int (*equal)(void *x, void *y))
{
    size_t i;
    if (x == y)
        return 1;
    if (x == NULL || y == NULL)
        return 0;
    if (x->len != y->len)
        return 0;
    if (x->type_size != y->type_size)
        return 0;
    if (x->len == 0)
        return 1;
    if (equal){
        for (i = 0; i < x->len; i++){
            if (!equal(array_at(x, i), array_at(y, i)))
                return 0;
        }
    }
    else{
        if (memcmp(x->array, y->array, x->type_size * x->len) != 0){
            return 0;
        }
    }
    return 1;
}

static 
int array_mem_move(Array *a, size_t new_cap)
{
    void *new_array = malloc(a->type_size * new_cap);
    if (new_array){
        if (a->len){
            memcpy(new_array, a->array, a->type_size * (a->len > new_cap ? new_cap:a->len));
        }
        if (a->array){
            free(a->array);
        }
        a->array = new_array;
        a->capacity = new_cap;
        return 1;
    }
    return 0;
}

int array_reserve(Array *a, size_t capacity)
{
    if (capacity <= a->len)
        return 1;
    
    return array_mem_move(a, capacity);
}

static inline
int array_ensure_cap(Array *a, size_t new_len)
{
    if (new_len <= a->capacity)
        return 1;
    
    // double origin
    size_t new_cap = (a->capacity << 1) > new_len ? a->capacity << 1 : new_len;
    return array_mem_move(a, new_cap);
}

void array_push(Array *a, void *item)
{
    a->len++;
    array_ensure_cap(a, a->len);
    memcpy((void *)(((uint8_t *)a->array) + (a->len - 1) * a->type_size), item, a->type_size);
}

