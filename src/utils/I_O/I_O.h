#ifndef _I_O_H
#define _I_O_H

#include <stdint.h>
#include "utils/args/args.h"

typedef struct file {
    uint8_t *body;
    const char *extension;
    uint64_t size;
} file_t;

typedef struct I_O_resources {
    int out_fd;
    file_t *in_file;
} I_O_resources_t;

int
open_I_O_resources(I_O_resources_t *resources, stegobmp_args_t args);

void
close_I_O_resources(I_O_resources_t *resources);


#endif