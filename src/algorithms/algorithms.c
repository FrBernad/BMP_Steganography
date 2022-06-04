#include <sys/stat.h>
#include <stdio.h>
#include "algorithms/algorithms.h"

enum bits_per_byte {
    LSB1_BITS = 1,
    LSB4_BITS = 4,
    LSBI_BITS = 1
};

static int
LSB1_embed(bmp_t *bmp, I_O_resources_t resources);

static int
LSB1_extract(bmp_t *bmp, I_O_resources_t resources);

static int
LSB4_embed(bmp_t *bmp, I_O_resources_t resources);

static int
LSB4_extract(bmp_t *bmp, I_O_resources_t resources);

static int
LSBI_embed(bmp_t *bmp, I_O_resources_t resources);

static int
LSBI_extract(bmp_t *bmp, I_O_resources_t resources);

static uint64_t
check_size(uint8_t bits_per_byte, uint32_t bmp_size, file_t *in_file);

static int (*embed_algorithms[3])(bmp_t *bmp, I_O_resources_t resources) = {
        LSB1_embed, LSB4_embed, LSBI_embed
};

static int (*extract_algorithms[3])(bmp_t *bmp, I_O_resources_t resources) = {
        LSB1_extract, LSB4_extract, LSBI_extract
};

int
embed(steg_algorithm_t algorithm, bmp_t *bmp, I_O_resources_t resources) {
    return embed_algorithms[algorithm](bmp, resources);
}

int
extract(steg_algorithm_t algorithm, bmp_t *bmp, I_O_resources_t resources) {
    return extract_algorithms[algorithm](bmp, resources);
}

static int
LSB1_embed(bmp_t *bmp, I_O_resources_t resources) {

    if (!check_size(LSB1_BITS, bmp->file_header.size, resources.in_file)) {
        return -1;
    }
    return 1;
}

static int
LSB1_extract(bmp_t *bmp, I_O_resources_t resources) {
    return 1;
}

static int
LSB4_embed(bmp_t *bmp, I_O_resources_t resources) {

    if (!check_size(LSB4_BITS, bmp->file_header.size, resources.in_file)) {
        return -1;
    }
    return 1;
}

static int
LSB4_extract(bmp_t *bmp, I_O_resources_t resources) {
    return 1;
}

static int
LSBI_embed(bmp_t *bmp, I_O_resources_t resources) {

    if (!check_size(LSBI_BITS, bmp->file_header.size, resources.in_file)) {
        return -1;
    }
    return 1;
}

static int
LSBI_extract(bmp_t *bmp, I_O_resources_t resources) {
    return 1;
}

static uint64_t
check_size(uint8_t bits_per_byte, uint32_t bmp_size, file_t *in_file) {

    if (bmp_size < (in_file->size * sizeof(uint8_t) / bits_per_byte)) {
        fprintf(stderr, "Cannot embed, file size is not big enough");
        return 0;
    }

    return 1;
}
