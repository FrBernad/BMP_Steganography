#ifndef _I_O_H
#define _I_O_H

#include <stdint.h>
#include "utils/args/args.h"
#include "bmp_factory/bmp_factory.h"

#define REAL_SIZE_BYTES 4
#define MAX_EXTENSION_SIZE 10
#define MAX_FILENAME_SIZE 50

typedef struct extracted_data {
    uint8_t *body;
    uint8_t *extension;
    uint8_t *file_name;
    uint32_t body_size;
} extracted_data_t;

typedef struct stego_data {
    uint8_t *data;
    uint32_t size;
} stego_data_t;

typedef struct I_O_resources {
    int out_fd;
    extracted_data_t *extracted_data;
    stego_data_t *stego_data;
} I_O_resources_t;

I_O_resources_t *
open_I_O_resources(stegobmp_args_t args);

int
generate_embedded_bmp(bmp_t *bmp, I_O_resources_t *resources);

int
generate_extracted_file(extracted_data_t *extracted_data, stegobmp_args_t args);

int
init_extracted_data(uint32_t size, extracted_data_t *extracted_data);

void
close_I_O_resources(I_O_resources_t *resources);


#endif