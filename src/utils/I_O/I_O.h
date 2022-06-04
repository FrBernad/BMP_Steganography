#ifndef _I_O_H
#define _I_O_H

#include <stdint.h>
#include "utils/args/args.h"
#include "bmp_factory/bmp_factory.h"

typedef struct file {
    uint8_t *body;
    const char *extension;
    uint32_t body_size;
} file_t;

typedef struct stego_data {
    uint8_t *data;
    uint32_t size;
} stego_data_t;

typedef struct I_O_resources {
    int out_fd;
    stego_data_t *stego_data;
} I_O_resources_t;

int
open_I_O_resources(I_O_resources_t *resources, stegobmp_args_t args);

int
generate_embedded_bmp(bmp_t * bmp,I_O_resources_t resources);

void
close_I_O_resources(I_O_resources_t *resources);


#endif