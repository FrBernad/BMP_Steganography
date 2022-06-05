#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <string.h>
#include "bmp_factory/bmp_factory.h"
#include "utils/logger/logger.h"

bmp_t *
create_bmp(const char *file_name) {

    int bmp_fd = open(file_name, O_RDONLY);
    if (bmp_fd == -1) {
        log_error("unable to open %s\n", file_name);
        return NULL;
    }

    bmp_t *bmp = calloc(1, sizeof(bmp_t));
    if (bmp == NULL) {
        close(bmp_fd);
        log_error("memory allocation error");
        return NULL;
    }

    bmp->raw_file_header = malloc(sizeof(uint8_t) * BMP_FILE_HEADER_SIZE);
    if (bmp->raw_file_header == NULL) {
        close(bmp_fd);
        free_bmp(bmp);
        log_error("memory allocation error");
        return NULL;
    }
    if (read(bmp_fd, bmp->raw_file_header, BMP_FILE_HEADER_SIZE) < 0) {
        close(bmp_fd);
        free_bmp(bmp);
        log_error("error reading file header");
        return NULL;
    }
    memcpy(&bmp->file_header, bmp->raw_file_header, BMP_FILE_HEADER_SIZE);

    bmp->raw_info_header = malloc(sizeof(uint8_t) * BMP_INFO_HEADER_SIZE);
    if (bmp->raw_info_header == NULL) {
        close(bmp_fd);
        free_bmp(bmp);
        log_error("memory allocation error");
        return NULL;
    }
    if (read(bmp_fd, bmp->raw_info_header, BMP_INFO_HEADER_SIZE) < 0) {
        close(bmp_fd);
        free_bmp(bmp);
        log_error("error reading info header");
        return NULL;
    }
    memcpy(&bmp->info_header, bmp->raw_info_header, BMP_INFO_HEADER_SIZE);

    bmp->pixel_array = malloc(bmp->info_header.image_size * sizeof(uint8_t));
    if (bmp->pixel_array == NULL) {
        log_error("memory error");
        close(bmp_fd);
        free_bmp(bmp);
        return NULL;
    }

    if (read(bmp_fd, bmp->pixel_array, bmp->info_header.image_size) < 0) {
        close(bmp_fd);
        free_bmp(bmp);
        log_error("error reading pixel array");
        return NULL;
    }

    close(bmp_fd);

    return bmp;
}

void
free_bmp(bmp_t *bmp) {

    if (bmp) {
        if (bmp->pixel_array) {
            free(bmp->pixel_array);
        }
        if (bmp->raw_file_header) {
            free(bmp->raw_file_header);
        }
        if (bmp->raw_info_header) {
            free(bmp->raw_info_header);
        }
        free(bmp);
    }
}
