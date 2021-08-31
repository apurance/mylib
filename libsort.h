#ifndef LIBSORT_H
#define LIBSORT_H

#include <stddef.h>

void select_sort(void *array, size_t len, size_t type_size,
                 int (*fn_cmp)(void *x, void *y),
                 void (*fn_swap)(void *x, void *y));

#endif