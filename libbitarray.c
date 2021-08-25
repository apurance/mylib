#include "libbitarray.h"
#include <stdlib.h>
#include <string.h>

#define BITARRAY_TAIL_NORM(ba) ((ba)->array[(ba)->num-1] &= mask_norm[(ba)->bit])

static uint8_t mask_norm[8] = {
    0b00000001,
    0b00000011,
    0b00000111,
    0b00001111,
    0b00011111,
    0b00111111,
    0b01111111,
    0b11111111
};

static uint8_t mask[8] = {
    0b00000000,
    0b00000001,
    0b00000011,
    0b00000111,
    0b00001111,
    0b00011111,
    0b00111111,
    0b01111111,
};

static uint8_t lookup[16] = {
    0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
    0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf,
};

/*
static uint8_t nibble_count[16] = {
    0, 1, 1, 2, 1, 2, 2, 3,
    1, 2, 2, 3, 2, 3, 3, 4
};
*/

static uint8_t byte_count[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
};

void bitarray_init(Bitarray *ba, size_t len)
{
    if (len){
        ba->num = len >> 3;
        ba->bit = (len - 1) & 7;
        if (len & 7)
            ba->num++;
        ba->capacity = ba->num;
        ba->array = malloc(ba->capacity);
    }
    else{
        ba->num = 0;
        ba->bit = 7;
        ba->capacity = 0;
        ba->array = NULL;
    }
}

void bitarray_free(Bitarray *ba)
{
    if (ba->capacity)
        free(ba->array);
    ba->array = NULL;
    ba->num = 0;
    ba->bit = 7;
    ba->capacity = 0;
}

uint8_t* bitarray_move(Bitarray *ba)
{
    uint8_t *move = ba->array;
    ba->array = NULL;
    ba->num = 0;
    ba->bit = 7;
    ba->capacity = 0;
    return move;
}

uint8_t bitarray_equal(Bitarray *x, Bitarray *y)
{
    size_t i;
    if (x==y)
        return 1;
    if (x==NULL || y==NULL)
        return 0;
    if (x->num != y->num)
        return 0;
    if (x->num==0)
        return 1;
    if (x->bit != y->bit)
        return 0;
    BITARRAY_TAIL_NORM(x);
    BITARRAY_TAIL_NORM(y);
    for (i = 0; i < x->num; i++)
        if (x->array[i] != y->array[i])
            return 0;
    return 1;
}

void bitarray_endian_convert(const Bitarray *ba)
{
	uint8_t block;
    size_t i;

	for (i = 0; i < ba->num; i++){
        // Reverse the top and bottom nibble then swap them.
        block = ba->array[i];
        ba->array[i] = (lookup[block&0b1111] << 4) | lookup[block>>4];
    }
}

void bitarray_reserve(Bitarray *ba, size_t capacity)
{
    uint8_t *ptr;
    
    if (capacity <= ba->num)
        return;
    ptr = malloc(capacity);
    if (ba->num)
        memcpy(ptr, ba->array, ba->num);
    if (ba->capacity)
        free(ba->array);
    ba->array = ptr;
    ba->capacity = capacity;
}

static void bitarray_ensure_capacity(Bitarray *ba, size_t new_size)
{
    size_t new_cap;
    uint8_t *ptr;

    if (new_size <= ba->capacity)
        return;
    new_cap = (!ba->capacity) ? new_size : ba->capacity << 1;
    if (new_size > new_cap)
        new_cap = new_size;
    ptr = malloc(new_cap);
    if (ba->num)
        memcpy(ptr, ba->array, ba->num);
    if (ba->capacity)
        free(ba->array);
    ba->array = ptr;
    ba->capacity = new_cap;
}

void bitarray_resize(Bitarray *ba, size_t size)
{
    size_t new_num = size >> 3;
    uint8_t new_bit = (size - 1) & 7;

    if (size & 7)
        new_num++;
    if (new_num < ba->num)
    {
        ba->num = new_num;
        ba->bit = new_bit;
        return;
    }
    if (new_num == ba->num)
    {
        if (new_bit <= ba->bit)
        {
            ba->bit = new_bit;
            return;
        }
        BITARRAY_TAIL_NORM(ba);
        ba->bit = new_bit;
        return;
    }
    BITARRAY_TAIL_NORM(ba);
    bitarray_ensure_capacity(ba, new_num);
    memset(ba->array+ba->num, 0, new_num-ba->num);
    ba->num = new_num;
    ba->bit = new_bit;
}

void bitarray_copy(Bitarray *dba, const Bitarray *sba)
{
    bitarray_ensure_capacity(dba, sba->num);
    memcpy(dba->array, sba->array, sba->num);
    dba->num = sba->num;
    dba->bit = sba->bit;
}

void bitarray_set_zeros(Bitarray *ba, size_t len)
{
    size_t new_num = len >> 3;
    uint8_t new_bit = (len - 1) & 7;

    if (len & 7)
        new_num++;
    if (ba->capacity < new_num){
        if (ba->capacity)
            free(ba->array);
        ba->array = calloc(new_num, 1);
        ba->capacity = new_num;
    }
    else{
        memset(ba->array, 0, new_num);
    }
    ba->num = new_num;
    ba->bit = new_bit;
}

void bitarray_set_ones(Bitarray *ba, size_t len)
{
    size_t new_num = len >> 3;
    uint8_t new_bit = (len - 1) & 7;

    if (len & 7)
        new_num++;
    if (ba->capacity < new_num){
        if (ba->capacity)
            free(ba->array);
        ba->array = malloc(new_num);
        ba->capacity = new_num;
    }
    memset(ba->array, 255, new_num);
    BITARRAY_TAIL_NORM(ba);
    ba->num = new_num;
    ba->bit = new_bit;
}

void bitarray_set_rands(Bitarray *ba, size_t len, unsigned int seed)
{
    size_t new_num = len >> 3, i;
    uint8_t new_bit = (len - 1) & 7;

    if (seed != 0){
        srand(seed);
    }

    if (len & 7)
        new_num++;
    if (ba->capacity < new_num){
        if (ba->capacity)
            free(ba->array);
        ba->array = malloc(new_num);
        ba->capacity = new_num;
    }
    for (i = 0; i < new_num; i++){
        ba->array[i] = rand() / (RAND_MAX / 256 + 1);
    }
    BITARRAY_TAIL_NORM(ba);
    ba->num = new_num;
    ba->bit = new_bit;
}

void bitarray_set_str(Bitarray *ba, const char *str, size_t len)
{
    size_t new_num = len >> 3, i;
    uint8_t new_bit = (len - 1) & 7;

    if (len & 7)
        new_num++;
    if (ba->capacity < new_num){
        if (ba->capacity)
            free(ba->array);
        ba->array = malloc(new_num);
        ba->capacity = new_num;
    }
    for (i = 0; i < len; i++){
        switch(str[i]){
            case '0':
                bitarray_set_0(ba, i);
                break;
            case '1':
                bitarray_set_1(ba, i);
                break;
            default:
                bitarray_set_1(ba, i);
        }
    }
    BITARRAY_TAIL_NORM(ba);
    ba->num = new_num;
    ba->bit = new_bit;
}

void bitarray_set_memory(Bitarray *ba, const uint8_t *mem, size_t len)
{
    size_t new_num = len >> 3;
    uint8_t new_bit = (len - 1) & 7;

    if (len & 7)
        new_num++;
    if (ba->capacity)
        free(ba->array);
    ba->array = mem;
    ba->capacity = new_num;
    ba->num = new_num;
    ba->bit = new_bit;
}

size_t bitarray_count(const Bitarray *ba, size_t start, size_t end)
{
    size_t count = 0, i, j;
    uint8_t block;
    if (end <= start)
        return 0;
    i = start >> 3; j = end >> 3;
    if (i == j){
        block = ba->array[i];
        block &= ~mask[start & 7];
        block &= mask[end & 7];
        return byte_count[block];
    }
    count += byte_count[ba->array[i] & (~mask[start & 7])];
    for (i++; i < j; i++){
        count += byte_count[ba->array[i]];
    }
    count += byte_count[ba->array[j] & mask[end & 7]];
    return count;
}

void bitarray_push(Bitarray *ba, uint8_t b)
{
    ba->bit = (ba->bit + 1) & 7;
    if (ba->bit){
        uint8_t mask_set = 1 << ba->bit;
        if (b)
            ba->array[ba->num-1] |= mask_set;
        else
            ba->array[ba->num-1] &= ~mask_set;
    }
    else{
        bitarray_ensure_capacity(ba, ba->num+1);
        ba->array[ba->num++] = b;
    }
}

void bitarray_repeat_push(Bitarray *ba, uint8_t b, size_t repetation)
{
    size_t a_num;  
    
    if (b)
        ba->array[ba->num-1] |= ~mask_norm[ba->bit];
    else
        ba->array[ba->num-1] &= mask_norm[ba->bit];
    if (repetation <= 7 - ba->bit){    
        ba->bit += repetation;
        return;
    }
    repetation -= 7 - ba->bit;
    a_num = repetation >> 3;
    if (repetation & 7)
        a_num++;
    bitarray_ensure_capacity(ba, ba->num + a_num);
    memset(ba->array+ba->num, b?255:0, a_num);
    ba->num += a_num;
    ba->bit = (repetation - 1) & 7;
}

void bitarray_push_ba(Bitarray *ba, const Bitarray *pba)
{
    uint8_t x, y, i, j;
    
    bitarray_ensure_capacity(ba, ba->num + pba->num);
    if (ba->bit == 7){
        memcpy(ba->array + ba->num, pba->array, pba->num);
        ba->bit = pba->bit;
        ba->num += pba->num;
        return;
    }
/*  
    7654 3210
 0b xxxx xxxx
          ^
          | 
          bit = 2
          _y_ = 3
    __x___    = 5
*/
    y = ba->bit + 1;
    x = 8 - y;
    BITARRAY_TAIL_NORM(ba);
    ba->array[ba->num-1] |= pba->array[0] << y;
    for (i = 0, j = ba->num; i < pba->num - 1; i++, j++)
        ba->array[j] = (pba->array[i] >> x) | (pba->array[i+1] << y);
    if ((pba->bit + 1) > x){
        ba->array[j] = pba->array[i] >> x;
        ba->bit = pba->bit - x;
        ba->num = j + 1;
        return;
    }
    ba->bit = pba->bit + y;
    ba->num = j;
}

void bitarray_sub_set(Bitarray *ba, uint8_t b, size_t start, size_t end)
{
    size_t i, j;

    if (end <= start)
        return;
    i = start >> 3; j = end >> 3;
    if (i == j){
        if (b)
            ba->array[i] |= ~mask[start & 7] & mask[end & 7];
        else
            ba->array[i] &= mask[start & 7] | ~mask[end & 7];
        return;
    }
    if (b){
        ba->array[i] |= ~mask[start & 7];
        ba->array[j] |= mask[end & 7];
    }
    else{
        ba->array[i] &= mask[start & 7];
        ba->array[j] &= ~mask[end & 7];
    }
    i++;
    if (j > i)
        memset(ba->array+i, b?255:0, j-i);
}

size_t bitarray_get_str(const Bitarray *ba, char *str)
{
    size_t i;
    size_t len = bitarray_len(ba);
    for (i = 0; i < len; i++){
        str[i] = bitarray_get(ba, i) + '0';
    }
    str[i] = 0;
    return len;
}
