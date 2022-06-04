#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "utils/I_O/I_O.h"
#include "utils/logger/logger.h"

static const char *
get_filename_ext(const char *filename);

int
open_I_O_resources(I_O_resources_t *resources, stegobmp_args_t args) {
    resources->out_fd = -1;
    resources->in_file = NULL;

    int out_file_fd = open(args.out_file, O_RDWR | O_CREAT | O_TRUNC, 0775);
    if (out_file_fd == -1) {
        log_error("Unable to create out file %s\n", args.out_file);
        return -1;
    }
    resources->out_fd = out_file_fd;

    if (args.embed) {
        int in_file_fd = open(args.in_file, O_RDONLY);
        if (in_file_fd == -1) {
            close_I_O_resources(resources);
            log_error("unable to open in file %s\n", args.in_file);
            return -1;
        }
        struct stat st;
        fstat(in_file_fd, &st);

        uint64_t file_size = (uint64_t) st.st_size;

        resources->in_file = calloc(1, sizeof(file_t));
        if (resources->in_file == NULL) {
            close_I_O_resources(resources);
            return -1;
        }

        resources->in_file->size = file_size;
        resources->in_file->body = malloc(sizeof(uint8_t) * file_size);
        if (resources->in_file->body == NULL) {
            close_I_O_resources(resources);
            return -1;
        }
        if (read(in_file_fd, resources->in_file->body, file_size) < 0) {
            log_error("unable to read in file %s\n", args.in_file);
            close_I_O_resources(resources);
            close(in_file_fd);
            return -1;
        }
        resources->in_file->extension = get_filename_ext(args.in_file);

        close(in_file_fd);
    }

    return 1;
}

void
close_I_O_resources(I_O_resources_t *resources) {
    if (resources->out_fd) {
        close(resources->out_fd);
    }

    if (resources->in_file) {
        if (resources->in_file->body) {
            free(resources->in_file->body);
        }
        free(resources->in_file);
    }
}

static const char *
get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}