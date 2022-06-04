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

static uint32_t
get_required_bytes(uint8_t bits_per_byte, uint32_t bmp_size, uint32_t file_size);

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
// IN_FILE
//    11110000
// CARRIER
//    00001111
//    00000001
//    11111111
//    00000000

static int
LSB1_embed(bmp_t *bmp, I_O_resources_t resources) {
    stego_data_t *stego_data = resources.stego_data;
    uint32_t required_bytes = get_required_bytes(LSB1_BITS, bmp->info_header.image_size, stego_data->size);
    if (!required_bytes) {
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
LSB1_extract(bmp_t *bmp, I_O_resources_t resources) {

    // Extract size of the file
    uint32_t real_size_bytes = 8 * sizeof(uint32_t) / LSB1_BITS;
    uint32_t real_size = 0;
    for (uint32_t i = 0, k = 0, j = 8; i < real_size_bytes; ++i) {
        real_size |= (((uint32_t) (bmp->pixel_array[i] & 0x01)) << (8 * k + (j - 1)));
        j--;
        if (j == 0) {
            k++;
            j = 8;
        }
    }

    // Extract content of the file

    uint32_t real_body_bytes = 8 * real_size / LSB1_BITS;
    uint8_t *body_ptr = bmp->pixel_array + real_size_bytes;
    init_extracted_data(real_size, resources.extracted_data);
    for (uint32_t i = 0, k = 0, j = 8; i < real_body_bytes; i++) {
        resources.extracted_data->body[k] |= ((uint8_t) (body_ptr[i] & 0x01)) << (j - 1);
        j--;
        if (j == 0) {
            k++;
            j = 8;
        }
    }

    // Extract extension of the file
    uint8_t *ext = body_ptr + real_body_bytes;
    uint8_t parsed_ext = 0;

    for (uint32_t i = 0, k = 0, j = 8; !parsed_ext; i++) {
        resources.extracted_data->extension[k] |= ((uint8_t) (ext[i] & 0x01)) << (j - 1);
        j--;
        if (j == 0) {
            if (resources.extracted_data->extension[k] == 0) {
                parsed_ext = 1;
            }
            k++;
            j = 8;
        }
    }

    return 1;
}

static int
LSB4_embed(bmp_t *bmp, I_O_resources_t resources) {
    stego_data_t *stego_data = resources.stego_data;
    uint32_t required_bytes = get_required_bytes(LSB4_BITS, bmp->info_header.image_size, stego_data->size);
    if (!required_bytes) {
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
    stego_data_t *stego_data = resources.stego_data;
    uint32_t required_bytes = get_required_bytes(LSBI_BITS, bmp->info_header.image_size, stego_data->size);
    if (!required_bytes) {
        return -1;
    }
    return 1;
}

static int
LSBI_extract(bmp_t *bmp, I_O_resources_t resources) {
    return 1;
}

static uint32_t
get_required_bytes(uint8_t bits_per_byte, uint32_t bmp_size, uint32_t file_size) {

    uint32_t required_bytes = file_size * sizeof(uint8_t) / bits_per_byte;

    if (bmp_size < required_bytes) {
        fprintf(stderr, "Cannot embed, file body_size is not big enough");
        return 0;
    }

    return required_bytes;
}
