//#define _CRT_SECURE_NO_WARNINGS
#define TEST

#include "libbmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint32_t pixel_size[] = {1, 4, 4};

void img_init(Img *img, uint32_t width, uint32_t height, enum img_type type)
{
    img->width = width;
    img->height = height;
    img->type = type;
    img->pixel_size = pixel_size[type];
    img->pixel = malloc(pixel_size[type] * width * height);
}

void img_free(Img *img)
{
    free(img->pixel);
    img->width = 0;
    img->height = 0;
    img->type = 0;
    img->pixel_size = 0;
    img->pixel = NULL;
}

static inline uint8_t rgb2gray(uint8_t blue, uint8_t green, uint8_t red)
{
    return (red * 299
          + green * 587
          + blue * 114
          + 500) / 1000;
}

enum bmp_error bmp_load(Img *img, const char *file_path, int isgray){
    FILE *fp = NULL;
    uint16_t signature;
    BmpFileHeader bfh;
    BmpInfoheader bih;
    uint8_t bottom_to_up = 1;
    uint8_t *palette8 = NULL;
    uint32_t *palette32 = NULL;
    uint8_t *pixel_array = NULL;
    enum bmp_error error_code = BMP_OK;

    fp = fopen(file_path, "rb");
    if (NULL == fp){
        return BMP_FILEERR;
    }
    
    // read signature (magic number)
    if (1 != fread(&signature, sizeof(signature), 1, fp)){
        error_code = BMP_FILEERR;
        goto BMP_ERR_L;
    }
    // verify signature
    if (0x4d42 != signature){
        error_code = BMP_UNSPPORT;
        goto BMP_ERR_L;
    }
    // read file header
    if (1 != fread(&bfh, sizeof(BmpFileHeader), 1, fp)){
        error_code = BMP_FILEERR;
        goto BMP_ERR_L;
    }

#ifdef TEST
    printf("file size: %u\n", bfh.file_size);
    printf("reserved: %u\n", bfh.reserved);
    printf("file_offset : %u\n", bfh.file_offset);
    printf("\n");
#endif

    // read info header
    if (1 != fread(&bih, sizeof(BmpInfoheader), 1, fp)){
        error_code = BMP_FILEERR;
        goto BMP_ERR_L;
    }
    if (bih.height < 0){
        bih.height *= -1;
        bottom_to_up = 0;
    }
    uint32_t rowsize = ((bih.bits_p_pixel * bih.width + 31) >> 5) << 2;

#ifdef TEST
    printf("header_size = %u\n", bih.header_size);
    printf("width = %u\n", bih.width);
    printf("height = %d\n", bih.height);
    printf("planes = %u\n", bih.planes);
    printf("bits_p_pixel = %u\n", bih.bits_p_pixel);
    printf("compression = %u\n", bih.compression);
    printf("image_size = %u\n", bih.image_size);
    printf("x_p_meter = %u\n", bih.x_p_meter);
    printf("y_p_meter = %u\n", bih.y_p_meter);
    printf("colors_in_colortable = %u\n", bih.colors_in_colortable);
    printf("bottom to up: %u\n", bottom_to_up);
#endif

    if (bih.width == 0 || bih.height == 0 || bih.bits_p_pixel == 0 || bih.image_size < rowsize * bih.height) {
        error_code = BMP_INVALID;
        goto BMP_ERR_L;
    }


    switch (bih.header_size){
        case 12: // BITMAPCOREHEADER
        case 64: // OS22XBITMAPHEADER
        case 16: // OS22XBITMAPHEADER
            error_code = BMP_UNSPPORT;
            goto BMP_ERR_L;
        case 40: // BITMAPINFOHEADER
        case 52: // BITMAPV2INFOHEADER
        case 56: // BITMAPV3INFOHEADER
        case 108: // BITMAPV4HEADER
        case 124: // BITMAPV5HEADER
            break;
        default:
            error_code = BMP_UNSPPORT;
            goto BMP_ERR_L;
    }

    // read palette
    if (fseek(fp, bih.header_size - sizeof(BmpInfoheader), SEEK_CUR)){
        error_code = BMP_FILEERR;
        goto BMP_ERR_L;
    }
    if (bih.colors_in_colortable){
        palette32 = malloc(sizeof(uint32_t) * bih.colors_in_colortable);
        if (bih.colors_in_colortable != 
            fread(palette32, sizeof(uint32_t), bih.colors_in_colortable, fp)){
            error_code = BMP_FILEERR;
            goto BMP_ERR_L;
        }
    }
    else if (bih.bits_p_pixel <= 8){      
        bih.colors_in_colortable = 1U << bih.bits_p_pixel;
        palette32 = malloc(sizeof(uint32_t) * bih.colors_in_colortable);
        if (bih.colors_in_colortable !=
            fread(palette32, sizeof(uint32_t), bih.colors_in_colortable, fp)){
            error_code = BMP_FILEERR;
            goto BMP_ERR_L;
        }
    }
    
    // read pixel array
    if (fseek(fp, bfh.file_offset, SEEK_SET)){
        error_code = BMP_FILEERR;
        goto BMP_ERR_L;
    }
    pixel_array = malloc(bih.image_size);
    if (NULL == pixel_array) {
        error_code = BMP_ERR;
        goto BMP_ERR_L;
    }
    if (bih.image_size != fread(pixel_array, 1, bih.image_size, fp)){
        error_code = BMP_FILEERR;
        goto BMP_ERR_L;
    }
    fclose(fp);

    // palette cast (ignore alpha band)
    size_t i, j;
    if (isgray && bih.colors_in_colortable) {
        ARGB32* argb32p = (ARGB32*)palette32;
        palette8 = malloc(bih.colors_in_colortable);
        if (NULL == palette8) {
            error_code = BMP_ERR;
            goto BMP_ERR_L;
        }
        for (i = 0; i < bih.colors_in_colortable; i++) {
            palette8[i] = rgb2gray(argb32p[i].blue,
                argb32p[i].green,
                argb32p[i].red);
        }
        free(palette32);
        palette32 = NULL;
    }
    // init img
    img_init(img, bih.width, bih.height, isgray?IMG_GRAY8:IMG_RGB24);
    // get pixel
    size_t i_offset;
    switch (bih.bits_p_pixel){
        case 1:
            
            break;
        case 2:

            break;
        case 4:

            break;
        case 8:
            for (i = 0; i < bih.height; i++){
                if (bottom_to_up)
                    i_offset = (bih.height - 1 - i) * bih.width;
                else
                    i_offset = i * bih.width;
                if (isgray) {
                    for (j = 0; j < bih.width; j++) {
                        ((uint8_t *)(img->pixel))[i_offset + j] = \
                            palette8[pixel_array[i * rowsize + j]];
                    }
                }
                else {
                    for (j = 0; j < bih.width; j++) {
                        ((uint32_t *)(img->pixel))[i_offset + j] = \
                            palette32[pixel_array[i * rowsize + j]];
                    }
                }
            }
            break;
        case 16:

            break;
        case 24:
            for (i = 0; i < bih.height; i++){
                if (bottom_to_up)
                    i_offset = (bih.height - 1 - i) * bih.width;
                else
                    i_offset = i * bih.width;
                if (isgray){
                    for (j = 0; j < bih.width; j++){
                        ((uint8_t *)(img->pixel))[i_offset + j] = \
                            rgb2gray(pixel_array[i*rowsize + j*3],
                                     pixel_array[i*rowsize + j*3 + 1],
                                     pixel_array[i*rowsize + j*3 + 2]);
                    }
                }
                else{
                    for (j = 0; j < bih.width; j++)
                        memcpy((uint8_t *)img->pixel + (i_offset + j) * 4,
                                pixel_array + i*rowsize + j*3,
                                3);
                }
            }
            break;
    }

BMP_ERR_L:
    if (fp)
        fclose(fp);
    if (palette8)
        free(palette8);
    if (palette32)
        free(palette32);
    if (pixel_array)
        free(pixel_array);
    return error_code;
}

enum bmp_error bmp_save(Img* img, const char* file_path)
{
    uint32_t DPI = 96, rowsize, i, j;
    FILE *fp;
    uint16_t signature = 0x4d42;
    BmpFileHeader bfh;
    BmpInfoheader bih;
    ARGB32 *palette32 = NULL;
    uint8_t* pixel_array = NULL;
    enum bmp_error error_code = BMP_OK;

    // create BmpInfoheader
    bih.header_size = 40;
    bih.width = img->width;
    bih.height = img->height;
    bih.planes = 1;
    switch (img->type) {
    case IMG_GRAY8: 
        bih.bits_p_pixel = 8;
        bih.compression = BI_RGB;
        break;
    case IMG_RGB24: 
        bih.bits_p_pixel = 24;
        bih.compression = BI_RGB;
        break;
    case IMG_ARGB32:
        bih.bits_p_pixel = 32;
        bih.compression = BI_BITFIELDS;
        break;
    default:
        bih.bits_p_pixel = 0;
        bih.compression = BI_RGB;
    }
    rowsize = ((bih.bits_p_pixel * bih.width + 31) >> 5) << 2;
    bih.image_size = rowsize * bih.height;
    bih.x_p_meter = DPI * 39.3701;
    bih.y_p_meter = DPI * 39.3701;
    if (bih.bits_p_pixel <= 8) {
        bih.colors_in_colortable = 1U << bih.bits_p_pixel;
    }
    else {
        bih.colors_in_colortable = 0;
    }
    bih.important_color_count = 0;

    // create BmpFileHeader
    bfh.file_offset = 14 + 40 + bih.colors_in_colortable;
    bfh.file_size = bfh.file_offset + bih.image_size;
    bfh.reserved = 0;

    // create palette
    if (bih.colors_in_colortable) {
        palette32 = malloc(sizeof(ARGB32) * bih.colors_in_colortable);
        if (NULL == palette32) {
            error_code = BMP_ERR;
            goto BMP_ERR_L;
        }
        for (i = 0; i < bih.colors_in_colortable; i++) {
            palette32[i].blue = i;
            palette32[i].green = i;
            palette32[i].red = i;
            palette32[i].alpha = 0;
        }
    }

    // create pixel array
    pixel_array = malloc(bih.image_size);
    if (NULL == pixel_array) {
        error_code = BMP_ERR;
        goto BMP_ERR_L;
    }
    switch (bih.bits_p_pixel) {
    case 8:
        for (i = 0; i < bih.height; i++) {
            memcpy(pixel_array + i * rowsize, (uint8_t *)img->pixel + (bih.height - 1 - i) * bih.width, bih.width);
        }
        break;
    case 24:
        for (i = 0; i < bih.height; i++) {
            for (j = 0; j < bih.width; j++) {
                memcpy(pixel_array + i * rowsize + j * 3, (uint8_t*)img->pixel + ((bih.height - 1 - i) * bih.width + j) * 4, 3);
            }
        }
        break;
    }

    // open file
    fp = fopen(file_path, "wb");
    if (NULL == fp) {
        error_code = BMP_FILEERR;
        goto BMP_ERR_L;
    }
    if (1 != fwrite(&signature, sizeof(signature), 1, fp)) {
        error_code = BMP_FILEERR;
        goto BMP_ERR_L;
    }
    if (1 != fwrite(&bfh, sizeof(bfh), 1, fp)) {
        error_code = BMP_FILEERR;
        goto BMP_ERR_L;
    }
    if (1 != fwrite(&bih, sizeof(bih), 1, fp)) {
        error_code = BMP_FILEERR;
        goto BMP_ERR_L;
    }
    if (palette32) {
        if (bih.colors_in_colortable != fwrite(palette32, sizeof(ARGB32), bih.colors_in_colortable, fp)) {
            error_code = BMP_FILEERR;
            goto BMP_ERR_L;
        }
    }
    if (bih.image_size != fwrite(pixel_array, 1, bih.image_size, fp)) {
        error_code = BMP_FILEERR;
        goto BMP_ERR_L;
    }
    fclose(fp);

BMP_ERR_L:
    if (palette32)
        free(palette32);
    if (pixel_array)
        free(pixel_array);
    return error_code;
}

