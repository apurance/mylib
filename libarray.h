#ifndef LIBARRAY_H
#define LIBARRAY_H

#include <stddef.h>
#include <stdint.h>

typedef struct array{
    void *array;
    size_t len;
    size_t capacity;
    size_t type_size;
}Array;

static inline
size_t array_len(Array *a)
{
    return a->len;
}

static inline
void* array_at(const Array *a, size_t idx)
{
    return (void *)(((uint8_t *)a->array) + idx * a->type_size);
}
static inline
void* array_begin(const Array *a)
{
    return a->array;
}

static inline
void* array_end(const Array *a)
{
    if (a->len)
        return (void *)(((uint8_t *)a->array) + (a->len - 1) * a->type_size);
    return NULL;
}

static inline
void array_pop(Array *a)
{
    if (a->len)
        a->len--;
}

void array_init(Array *a, size_t len, size_t type_size);
void array_free(Array *a);
int array_reserve(Array *a, size_t capacity);
int array_equal(const Array *x, const Array *y, int (*equal)(void *x, void *y));
void array_push(Array *a, void *item);
void array_sort(const Array *a, void (*swap)(void *a, void *b), int ascend);


#define ARRAY_INIT(a, len, type_t) array_init(a, len, sizeof(type_t))
#define ARRAY_GET(a, idx, type_t) \
    ({ \
        type_t __atmp = *array_at(a, idx); \
        __atmp; \
    })
#define ARRAY_SET(a, idx, value) \
    ({ \
        typeof(value) *__atmpp = array_at(a, idx); \
        *__atmpp = value; \
    })


#endif