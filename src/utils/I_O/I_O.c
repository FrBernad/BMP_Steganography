#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "utils/I_O/I_O.h"
#include "utils/logger/logger.h"
#include "bmp_factory/bmp_factory.h"

#define REAL_SIZE_BYTES 4

static const char *
get_filename_ext(const char *filename);

static size_t
get_extension_size(const char *extension);

int
open_I_O_resources(I_O_resources_t *resources, stegobmp_args_t args) {
    resources->out_fd = -1;
    resources->stego_data = NULL;

    int out_file_fd = open(args.out_file, O_RDWR | O_CREAT | O_TRUNC, 0775);
    if (out_file_fd == -1) {
        log_error("Unable to create out file %s", args.out_file);
        return -1;
    }
    resources->out_fd = out_file_fd;

    if (args.embed) {
        int in_file_fd = open(args.in_file, O_RDONLY);
        if (in_file_fd == -1) {
            close_I_O_resources(resources);
            log_error("unable to open in file %s", args.in_file);
            return -1;
        }

        struct stat st;
        fstat(in_file_fd, &st);
        uint32_t file_size = (uint32_t) st.st_size;

        resources->stego_data = calloc(1, sizeof(stego_data_t));
        if (resources->stego_data == NULL) {
            log_error("memory error");
            close_I_O_resources(resources);
            return -1;
        }

        const char *extension = get_filename_ext(args.in_file);
        size_t extension_size = get_extension_size(extension);
        resources->stego_data->size = REAL_SIZE_BYTES + file_size + extension_size;
        resources->stego_data->data = malloc(sizeof(uint8_t) * resources->stego_data->size);
        if (resources->stego_data->data == NULL) {
            log_error("memory error");
            close_I_O_resources(resources);
            return -1;
        }
        //00 00 01 4D
        //4D 01 00 00
        // Se guarda en Big Endian
        uint8_t real_size[REAL_SIZE_BYTES] = {0};
        for (int i = REAL_SIZE_BYTES - 1; i >= 0; --i) {
            real_size[i] = (file_size >> (8 * i)) & 0x000000FF;
        }
        memcpy(resources->stego_data->data, real_size, sizeof(uint8_t) * REAL_SIZE_BYTES);

        if (read(in_file_fd, resources->stego_data->data + REAL_SIZE_BYTES, file_size) < 0) {
            log_error("unable to read in file %s", args.in_file);
            close_I_O_resources(resources);
            close(in_file_fd);
            return -1;
        }

        memcpy(resources->stego_data->data + REAL_SIZE_BYTES + file_size, extension, sizeof(uint8_t) * extension_size);

        close(in_file_fd);
    }

    return 1;
}

int
generate_embedded_bmp(bmp_t *bmp, I_O_resources_t resources) {
    if (write(resources.out_fd, bmp->raw_file_header, BMP_FILE_HEADER_SIZE) < 0) {
        log_error("unable to write output file header");
        return -1;
    }

    if (write(resources.out_fd, bmp->raw_info_header, BMP_INFO_HEADER_SIZE) < 0) {
        log_error("unable to write output info header");
        return -1;
    }

    if (write(resources.out_fd, bmp->pixel_array, bmp->info_header.image_size) < 0) {
        log_error("unable to write output pixel_array");
        return -1;
    }
    return 1;
}


void
close_I_O_resources(I_O_resources_t *resources) {
    if (resources->out_fd) {
        close(resources->out_fd);
    }

    if (resources->stego_data) {
        if (resources->stego_data->data) {
            free(resources->stego_data->data);
        }
        free(resources->stego_data);
    }
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