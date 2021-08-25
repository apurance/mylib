#ifndef LIBBMP_H
#define LIBBMP_H

#include <stdint.h>
#include <stddef.h>

enum bmp_error{
    BMP_OK,
    BMP_FILEERR,
    BMP_UNSPPORT,
    BMP_INVALID,
    BMP_ERR
};

enum bmp_compression{
    BI_RGB,
    BI_RLE8,
    BI_RLE4,
    BI_BITFIELDS,
    BI_JPEG,
    BI_PNG,
    BI_ALPHABITFIELDS,
    BI_CMYK,
    BI_CMYKRLE8,
    BI_CMYKRLE4
};

enum img_type{
    IMG_GRAY8,
    IMG_RGB24,
    IMG_ARGB32
};

typedef struct bmp_file_header{
  //uint16_t signature;
    uint32_t file_size;
    uint32_t reserved;
    uint32_t file_offset;
}BmpFileHeader;

typedef struct bmp_info_header{
    uint32_t header_size;
    uint32_t width;
     int32_t height;
    uint16_t planes; uint16_t bits_p_pixel;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_p_meter;
    uint32_t y_p_meter;
    uint32_t colors_in_colortable;
    uint32_t important_color_count;
}BmpInfoheader;

typedef struct pixel_argb_32{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
}ARGB32;

typedef struct pixel_rgba_32 {
    uint8_t alpha;
    uint8_t blue;
    uint8_t green;
    uint8_t red;
}RGBA32;

typedef struct img{
    uint32_t width;
    uint32_t height;
    enum img_type type;
    uint32_t pixel_size;
    void *pixel;
}Img;

void img_init(Img *img, uint32_t width, uint32_t height, enum img_type type);
void img_free(Img *img);
enum bmp_error bmp_load(Img *img, const char *file_path, int isgray);
enum bmp_error bmp_save(Img* img, const char* file_path);

#endif