#ifndef _ALGORITHMS_H
#define _ALGORITHMS_H

#include "bmp_factory/bmp_factory.h"
#include "utils/I_O/I_O.h"

typedef struct encryption {
    void (*enc)(int);
} encryption_t;

int embed(steg_algorithm_t steg_algorithm, bmp_t *bmp, I_O_resources_t resources);

int extract(steg_algorithm_t steg_algorithm, bmp_t *bmp, I_O_resources_t resources);

#endif
