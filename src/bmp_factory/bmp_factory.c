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
    if (bmp == 0) {
        close(bmp_fd);
        log_error("memory allocation error");
        return NULL;
    }

    if (read(bmp_fd, &bmp->file_header, BMP_FILE_HEADER_SIZE) < 0) {
        close(bmp_fd);
        destroy_bmp(bmp);
        log_error("error reading file header");
        return NULL;
    }

    if (read(bmp_fd, &bmp->info_header, BMP_INFO_HEADER_SIZE) < 0) {
        close(bmp_fd);
        destroy_bmp(bmp);
        log_error("error reading info header");
        return NULL;
    }

    bmp->pixel_array = malloc(bmp->info_header.image_size * sizeof(uint8_t));
    if (read(bmp_fd, bmp->pixel_array, bmp->info_header.image_size) < 0) {
        close(bmp_fd);
        destroy_bmp(bmp);
        log_error("error reading pixel array");
        return NULL;
    }

    close(bmp_fd);

    return bmp;
}

void
destroy_bmp(bmp_t *bmp) {
    free(bmp->pixel_array);
    free(bmp);
}
