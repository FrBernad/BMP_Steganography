#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "utils/I_O/I_O.h"
#include "utils/logger/logger.h"
#include "utils/encryption/encryption.h"
#include "bmp_factory/bmp_factory.h"

static const char *
get_filename_ext(const char *filename);

static size_t
get_extension_size(const char *extension);

static int
parse_file(extracted_data_t *extracted_data, uint8_t *plaintext);

static int
generate_extract_resources(I_O_resources_t *resources, stegobmp_args_t args);

static int
generate_embed_resources(I_O_resources_t *resources, stegobmp_args_t args);

I_O_resources_t *
open_I_O_resources(stegobmp_args_t args) {

    I_O_resources_t *resources = calloc(1, sizeof(I_O_resources_t));
    resources->out_fd = -1;

    if (args.embed) {
        if (generate_embed_resources(resources, args) < 0) {
            close_I_O_resources(resources);
            return NULL;
        }
    } else {
        if (generate_extract_resources(resources, args) < 0) {
            close_I_O_resources(resources);
            return NULL;
        }
    }

    return resources;
}

int
generate_embedded_bmp(bmp_t *bmp, I_O_resources_t *resources) {

    if (write(resources->out_fd, bmp->raw_file_header, BMP_FILE_HEADER_SIZE) < 0) {
        log_error("unable to write output file header");
        return -1;
    }

    if (write(resources->out_fd, bmp->raw_info_header, BMP_INFO_HEADER_SIZE) < 0) {
        log_error("unable to write output info header");
        return -1;
    }

    if (bmp->file_header.offset) {
        if (write(resources->out_fd, bmp->raw_offset, bmp->file_header.offset) < 0) {
            log_error("unable to write output offset");
            return -1;
        }
    }

    if (write(resources->out_fd, bmp->pixel_array, bmp->info_header.image_size) < 0) {
        log_error("unable to write output pixel_array");
        return -1;
    }
    return 1;
}

int
generate_extracted_file(extracted_data_t *extracted_data, stegobmp_args_t args) {

    uint8_t *plaintext = NULL;
    if (args.enc > 0) {
        int decrypted_data_size = decrypt(extracted_data->body,
                                          (int) extracted_data->body_size, (uint8_t *) args.pass,
                                          enc_algorithm_string(args.enc), chain_mode_string(args.mode),
                                          &plaintext);
        if (decrypted_data_size < 0) {
            return -1;
        }
        if (parse_file(extracted_data, plaintext) < 0) {
            free(plaintext);
            return -1;
        }
        free(plaintext);
    }

    char *extracted_filename = strcat((char *) extracted_data->file_name, (char *) extracted_data->extension);

    int out_file_fd = open(extracted_filename, O_RDWR | O_CREAT | O_TRUNC, 0775);
    if (out_file_fd == -1) {
        log_error("unable to create out file %s", extracted_filename);
        return -1;
    }

    if (write(out_file_fd, extracted_data->body, extracted_data->body_size) < 0) {
        log_error("unable to write output file body");
        return -1;
    }

    return 1;
}

int
init_extracted_data(uint32_t size, extracted_data_t *extracted_data) {

    extracted_data->body_size = size;

    extracted_data->body = calloc(size, sizeof(uint8_t));
    if (extracted_data->body == NULL) {
        log_error("memory allocation error");
        return -1;
    }

    return 1;
}

void
close_I_O_resources(I_O_resources_t *resources) {

    if (resources) {
        if (resources->out_fd) {
            close(resources->out_fd);
        }

        if (resources->stego_data) {
            if (resources->stego_data->data) {
                free(resources->stego_data->data);
            }
            free(resources->stego_data);
        }

        if (resources->extracted_data) {
            if (resources->extracted_data->body) {
                free(resources->extracted_data->body);
            }
            if (resources->extracted_data->extension) {
                free(resources->extracted_data->extension);
            }
            if (resources->extracted_data->file_name) {
                free(resources->extracted_data->file_name);
            }
            free(resources->extracted_data);
        }
        free(resources);
    }

}

static int
generate_extract_resources(I_O_resources_t *resources, stegobmp_args_t args) {
    resources->extracted_data = calloc(1, sizeof(extracted_data_t));
    if (resources->extracted_data == NULL) {
        log_error("memory allocation error");
        return -1;
    }

    resources->extracted_data->extension = calloc(MAX_EXTENSION_SIZE, sizeof(uint8_t));
    if (resources->extracted_data->extension == NULL) {
        log_error("memory allocation error");
        return -1;
    }

    resources->extracted_data->file_name = malloc(MAX_FILENAME_SIZE * sizeof(uint8_t));
    if (resources->extracted_data->extension == NULL) {
        log_error("memory allocation error");
        return -1;
    }
    strcpy((char *) resources->extracted_data->file_name, args.out_file);

    return 1;
}

static int
generate_embed_resources(I_O_resources_t *resources, stegobmp_args_t args) {

    int out_file_fd = open(args.out_file, O_RDWR | O_CREAT | O_TRUNC, 0775);
    if (out_file_fd == -1) {
        log_error("unable to create out file %s", args.out_file);
        return -1;
    }
    resources->out_fd = out_file_fd;

    int in_file_fd = open(args.in_file, O_RDONLY);
    if (in_file_fd == -1) {
        log_error("unable to open in file %s", args.in_file);
        return -1;
    }

    struct stat st;
    fstat(in_file_fd, &st);
    uint32_t file_size = (uint32_t) st.st_size;

    resources->stego_data = calloc(1, sizeof(stego_data_t));
    if (resources->stego_data == NULL) {
        log_error("memory allocation error");
        return -1;
    }

    const char *extension = get_filename_ext(args.in_file);
    size_t extension_size = get_extension_size(extension);
    resources->stego_data->size = REAL_SIZE_BYTES + file_size + extension_size;

    resources->stego_data->data = malloc(sizeof(uint8_t) * resources->stego_data->size);
    if (resources->stego_data->data == NULL) {
        log_error("memory error");
        return -1;
    }

    uint8_t real_size[REAL_SIZE_BYTES] = {0};
    for (int i = REAL_SIZE_BYTES - 1, j = 0; i >= 0; --i) {
        real_size[j++] = (file_size >> (8 * i)) & 0x000000FF;
    }

    memcpy(resources->stego_data->data, real_size, sizeof(uint8_t) * REAL_SIZE_BYTES);

    if (read(in_file_fd, resources->stego_data->data + REAL_SIZE_BYTES, file_size) < 0) {
        log_error("unable to read in file %s", args.in_file);
        close(in_file_fd);
        return -1;
    }

    memcpy(resources->stego_data->data + REAL_SIZE_BYTES + file_size, extension, sizeof(uint8_t) * extension_size);

    if (args.enc) {
        uint8_t *cyphertext = NULL;
        int encrypted_data_size = encrypt_data(resources->stego_data->data, (int) resources->stego_data->size,
                                               (uint8_t *) args.pass, enc_algorithm_string(args.enc),
                                               chain_mode_string(args.mode), &cyphertext);
        if (encrypted_data_size < 0) {
            return -1;
        }
        free(resources->stego_data->data);
        resources->stego_data->data = cyphertext;

        memmove(resources->stego_data->data + REAL_SIZE_BYTES, resources->stego_data->data, encrypted_data_size);

        uint8_t encrypted_data_size_array[REAL_SIZE_BYTES] = {0};
        for (int i = REAL_SIZE_BYTES - 1, j = 0; i >= 0; --i) {
            encrypted_data_size_array[j++] = (((uint32_t) encrypted_data_size) >> (8 * i)) & 0x000000FF;
        }

        memcpy(resources->stego_data->data, encrypted_data_size_array, REAL_SIZE_BYTES);
        resources->stego_data->size = encrypted_data_size + REAL_SIZE_BYTES;
    }

    return 1;
}

static const char *
get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return ".unknown";
    return dot;
}

static size_t
get_extension_size(const char *extension) {
    return strlen(extension) + 1;
}

static int
parse_file(extracted_data_t *extracted_data, uint8_t *plaintext) {

    uint32_t file_len = 0;
    for (uint32_t i = 0, k = 3; i < REAL_SIZE_BYTES; i++, k--) {
        file_len |= (((uint32_t) plaintext[i]) << (8 * k));
    }

    if (file_len > extracted_data->body_size) {
        return -1;
    }

    extracted_data->body_size = file_len;
    uint8_t *body_ptr = plaintext + REAL_SIZE_BYTES;
    for (uint32_t i = 0; i < extracted_data->body_size; i++) {
        extracted_data->body[i] = body_ptr[i];
    }

    uint8_t *ext = body_ptr + extracted_data->body_size;
    bool parsed_ext = false;
    for (uint32_t i = 0; !parsed_ext; i++) {
        if (i >= MAX_EXTENSION_SIZE) {
            return -1;
        }
        extracted_data->extension[i] = ext[i];
        if (ext[i] == 0) {
            parsed_ext = true;
        }
    }

    return 1;
}