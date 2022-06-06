#include <stdio.h>
#include <stdlib.h>
#include "algorithms/algorithms.h"

#define LSBI_COMBINATIONS 4

enum bits_per_byte {
    LSB1_BITS = 1,
    LSB4_BITS = 4,
    LSBI_BITS = 1
};

typedef struct LSBI_combination {
    uint32_t changed;
    uint32_t unchanged;
    bool invert;
} LSBI_combination_t;

static int
LSB1_embed(bmp_t *bmp, I_O_resources_t *resources);

static int
LSB1_extract(bmp_t *bmp, I_O_resources_t *resources, bool encrypted);

static int
LSB4_embed(bmp_t *bmp, I_O_resources_t *resources);

static int
LSB4_extract(bmp_t *bmp, I_O_resources_t *resources, bool encrypted);

static int
LSBI_embed(bmp_t *bmp, I_O_resources_t *resources);

static int
LSBI_extract(bmp_t *bmp, I_O_resources_t *resources, bool encrypted);

static uint32_t
check_required_bytes(uint8_t bits_per_byte, uint32_t bmp_size, uint32_t file_size);

static int (*embed_algorithms[3])(bmp_t *bmp, I_O_resources_t *resources) = {
        LSB1_embed, LSB4_embed, LSBI_embed
};

static int (*extract_algorithms[3])(bmp_t *bmp, I_O_resources_t *resources, bool encrypted) = {
        LSB1_extract, LSB4_extract, LSBI_extract
};

int
embed(steg_algorithm_t algorithm, bmp_t *bmp, I_O_resources_t *resources) {
    return embed_algorithms[algorithm - 1](bmp, resources);
}

int
extract(steg_algorithm_t algorithm, bmp_t *bmp, I_O_resources_t *resources, bool encrypted) {
    return extract_algorithms[algorithm - 1](bmp, resources, encrypted);
}

static int
LSB1_embed(bmp_t *bmp, I_O_resources_t *resources) {

    stego_data_t *stego_data = resources->stego_data;

    if (!check_required_bytes(LSB1_BITS, bmp->info_header.image_size, stego_data->size)) {
        return -1;
    }

    for (uint32_t i = 0, k = 0; i < stego_data->size; ++i) {
        uint8_t current_byte = stego_data->data[i];
        for (int j = 7; j >= 0; --j) {
            bmp->pixel_array[k] |= 0x01;
            bmp->pixel_array[k] &= (((current_byte >> j) & 0x01) | 0xFE);
            k++;
        }
    }

    return 1;
}

static int
LSB1_extract(bmp_t *bmp, I_O_resources_t *resources, bool encrypted) {

    // Extract size of the file
    uint32_t real_size_bytes = 8 * sizeof(uint32_t) / LSB1_BITS;
    uint32_t real_size = 0;

    for (uint32_t i = 0, k = 3, j = 8; i < real_size_bytes; ++i) {
        real_size |= (((uint32_t) (bmp->pixel_array[i] & 0x01)) << (8 * k + (j - 1)));
        j--;
        if (j == 0) {
            k--;
            j = 8;
        }
    }

    // Extract content of the file
    uint32_t real_body_bytes = 8 * real_size / LSB1_BITS;
    uint8_t *body_ptr = bmp->pixel_array + real_size_bytes;

    if (real_body_bytes > bmp->info_header.image_size) {
        return -1;
    }

    if (init_extracted_data(real_size, resources->extracted_data) < 0) {
        return -1;
    }

    for (uint32_t i = 0, k = 0, j = 8; i < real_body_bytes; i++) {
        resources->extracted_data->body[k] |= ((uint8_t) (body_ptr[i] & 0x01)) << (j - 1);
        j--;
        if (j == 0) {
            k++;
            j = 8;
        }
    }

    if (!encrypted) {
        // Extract extension of the file
        uint8_t *ext = body_ptr + real_body_bytes;
        uint8_t parsed_ext = 0;

        for (uint32_t i = 0, k = 0, j = 8; !parsed_ext; i++) {
            resources->extracted_data->extension[k] |= ((uint8_t) (ext[i] & 0x01)) << (j - 1);
            j--;
            if (j == 0) {
                if (resources->extracted_data->extension[k] == 0) {
                    parsed_ext = 1;
                }
                k++;
                j = 8;
            }
        }
    }


    return 1;
}

static int
LSB4_embed(bmp_t *bmp, I_O_resources_t *resources) {

    stego_data_t *stego_data = resources->stego_data;

    if (!check_required_bytes(LSB4_BITS, bmp->info_header.image_size, stego_data->size)) {
        return -1;
    }

    for (uint32_t i = 0, k = 0; i < stego_data->size; ++i) {
        uint8_t current_byte = stego_data->data[i];
        for (int j = 1; j >= 0; --j) {
            bmp->pixel_array[k] |= 0x0F;
            bmp->pixel_array[k] &= (((current_byte >> j * 4) & 0x0F) | 0xF0);
            k++;
        }
    }

    return 1;
}

static int
LSB4_extract(bmp_t *bmp, I_O_resources_t *resources, bool encrypted) {

    // Extract size of the file
    uint32_t real_size_bytes = 8 * sizeof(uint32_t) / LSB4_BITS;
    uint32_t real_size = 0;

    for (uint32_t i = 0, k = 3, j = 2; i < real_size_bytes; ++i) {
        real_size |= (((uint32_t) (bmp->pixel_array[i] & 0x0F)) << (8 * k + (j - 1) * 4));
        j--;
        if (j == 0) {
            k--;
            j = 2;
        }
    }

    // Extract content of the file
    uint32_t real_body_bytes = 8 * real_size / LSB4_BITS;
    uint8_t *body_ptr = bmp->pixel_array + real_size_bytes;

    if (real_body_bytes > bmp->info_header.image_size) {
        return -1;
    }

    if (init_extracted_data(real_size, resources->extracted_data) < 0) {
        return -1;
    }

    for (uint32_t i = 0, k = 0, j = 2; i < real_body_bytes; i++) {
        resources->extracted_data->body[k] |= ((uint8_t) (body_ptr[i] & 0x0F)) << ((j - 1) * 4);
        j--;
        if (j == 0) {
            k++;
            j = 2;
        }
    }

    if (!encrypted) {
        // Extract extension of the file
        uint8_t *ext = body_ptr + real_body_bytes;
        uint8_t parsed_ext = 0;

        for (uint32_t i = 0, k = 0, j = 2; !parsed_ext; i++) {
            resources->extracted_data->extension[k] |= ((uint8_t) (ext[i] & 0x0F)) << ((j - 1) * 4);
            j--;
            if (j == 0) {
                if (resources->extracted_data->extension[k] == 0) {
                    parsed_ext = 1;
                }
                k++;
                j = 2;
            }
        }
    }

    return 1;
}

static int
LSBI_embed(bmp_t *bmp, I_O_resources_t *resources) {

    stego_data_t *stego_data = resources->stego_data;
    // add one extra byte for altered combination
    if (!check_required_bytes(LSBI_BITS, bmp->info_header.image_size, stego_data->size + 1)) {
        return -1;
    }

    LSBI_combination_t combinations[LSBI_COMBINATIONS] = {0};

    for (uint32_t i = 0, k = LSBI_COMBINATIONS; i < stego_data->size; ++i) {
        uint8_t current_byte = stego_data->data[i];
        for (int j = 7; j >= 0; --j) {

            uint8_t combination = (bmp->pixel_array[k] >> 1) & 0x03;

            uint8_t bmp_lsb = bmp->pixel_array[k] & 0x01;
            uint8_t steg_lsb = (current_byte >> j) & 0x01;
            if (steg_lsb != bmp_lsb) {
                combinations[combination].changed += 1;
            } else {
                combinations[combination].unchanged += 1;
            }

            bmp->pixel_array[k] |= 0x01;
            bmp->pixel_array[k] &= (steg_lsb | 0xFE);
            k++;
        }
    }

    for (int i = 0; i < LSBI_COMBINATIONS; ++i) {
        if (combinations[i].changed > combinations[i].unchanged) {
            combinations[i].invert = true;
        }
    }

    // invert if condition is met
    for (uint32_t i = 0, k = LSBI_COMBINATIONS; i < stego_data->size; ++i) {
        for (int j = 7; j >= 0; --j) {
            uint8_t combination = (bmp->pixel_array[k] >> 1) & 0x03;
            if (combinations[combination].invert) {
                bmp->pixel_array[k] ^= 0x01;
            }
            k++;
        }
    }

    for (uint32_t i = 0; i < LSBI_COMBINATIONS; ++i) {
        bmp->pixel_array[i] |= 0x01;
        bmp->pixel_array[i] &= combinations[i].invert;
    }

    return 1;
}

static int
LSBI_extract(bmp_t *bmp, I_O_resources_t *resources, bool encrypted) {

    // Check inverted bits
    LSBI_combination_t combinations[LSBI_COMBINATIONS] = {0};

    for (int i = 0; i < LSBI_COMBINATIONS; ++i) {
        combinations[i].invert = bmp->pixel_array[i] & 0x01;
    }

    // Extract size of the file
    uint32_t real_size_bytes = 8 * sizeof(uint32_t) / LSBI_BITS;
    uint32_t real_size = 0;

    for (uint32_t i = LSBI_COMBINATIONS, k = 3, j = 8; i < real_size_bytes + LSBI_COMBINATIONS; ++i) {
        uint8_t combination = (bmp->pixel_array[i] >> 1) & 0x03;
        uint8_t current_byte = bmp->pixel_array[i];

        if (combinations[combination].invert) {
            current_byte ^= 0x01;
        }

        real_size |= (((uint32_t) (current_byte & 0x01)) << (8 * k + (j - 1)));
        j--;
        if (j == 0) {
            k--;
            j = 8;
        }
    }

    // Extract content of the file
    uint32_t real_body_bytes = 8 * real_size / LSBI_BITS;
    uint8_t *body_ptr = bmp->pixel_array + LSBI_COMBINATIONS + real_size_bytes;

    if (real_body_bytes > bmp->info_header.image_size) {
        return -1;
    }

    if (init_extracted_data(real_size, resources->extracted_data) < 0) {
        return -1;
    }

    for (uint32_t i = 0, k = 0, j = 8; i < real_body_bytes; i++) {
        uint8_t combination = (body_ptr[i] >> 1) & 0x03;
        uint8_t current_byte = body_ptr[i];

        if (combinations[combination].invert) {
            current_byte ^= 0x01;
        }

        resources->extracted_data->body[k] |= ((uint8_t) (current_byte & 0x01)) << (j - 1);
        j--;
        if (j == 0) {
            k++;
            j = 8;
        }
    }

    if (!encrypted) {
        // Extract extension of the file
        uint8_t *ext = body_ptr + real_body_bytes;
        uint8_t parsed_ext = 0;

        for (uint32_t i = 0, k = 0, j = 8; !parsed_ext; i++) {
            uint8_t combination = (ext[i] >> 1) & 0x03;
            uint8_t current_byte = ext[i];

            if (combinations[combination].invert) {
                current_byte ^= 0x01;
            }

            resources->extracted_data->extension[k] |= ((uint8_t) (current_byte & 0x01)) << (j - 1);
            j--;
            if (j == 0) {
                if (resources->extracted_data->extension[k] == 0) {
                    parsed_ext = 1;
                }
                k++;
                j = 8;
            }
        }
    }


    return 1;
}

static uint32_t
check_required_bytes(uint8_t bits_per_byte, uint32_t bmp_size, uint32_t file_size) {
    uint32_t required_bytes = file_size * sizeof(uint8_t) / bits_per_byte;
    if (bmp_size < required_bytes) {
        fprintf(stderr, "Cannot embed, file body_size must be bigger than %d", required_bytes);
        return 0;
    }

    return required_bytes;
}
