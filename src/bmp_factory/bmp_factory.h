#ifndef _BMP_FACTORY_H
#define _BMP_FACTORY_H

#include <stdint.h>

typedef enum {
    BMP_FILE_HEADER_SIZE = 14,
    BMP_INFO_HEADER_SIZE = 40
} headers_size;

typedef struct bmp_file_header {
    uint16_t type;
    uint32_t size;
    uint16_t reserved_1;
    uint16_t reserved_2;
    uint32_t offset;
} bmp_file_header_t;

typedef struct bmp_info_header {
    uint32_t size;
    uint32_t width; /*QUIZAS ES SIGNED*/
    uint32_t height;/*QUIZAS ES SIGNED*/
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_pixels_per_meter;
    uint32_t y_pixels_per_meter;
    uint32_t clr_used;
    uint32_t clr_important;
} bmp_info_header_t;


typedef struct bmp {
    bmp_file_header_t file_header;
    bmp_info_header_t info_header;
    uint8_t *pixel_array;
} bmp_t;

bmp_t *
create_bmp(const char *file_name);

void
free_bmp(bmp_t *bmp);

#endif
