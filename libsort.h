#ifndef LIBSORT_H
#define LIBSORT_H

#include <stddef.h>

typedef void (*fn_swap)(void *x, void *y);
typedef int (*fn_cmp)(void *x, void *y);

void select_sort(void *array, size_t len, size_t type_size,
                 fn_cmp cmp, fn_swap swap);
void quick_sort(void *array, size_t len, size_t type_size,
                fn_cmp cmp, fn_swap swap);

/* 
 * @param tmp must not be NULL, its size should at least >= type_size
 */
void insert_sort(void *array, size_t len, size_t type_size,
                 fn_cmp cmp, void *tmp);

void merge_sort(void *array, size_t len, size_t type_size,
                fn_cmp cmp, void *aux);
void shuffle(void *array, size_t len, size_t type_size,
                 size_t (*randint)(size_t bound),
                 fn_swap swap);
int sort_check(void *array, size_t len, size_t type_size,
               fn_cmp cmp);

#endif