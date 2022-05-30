#include <stdio.h>
#include "utils/args/args.h"
#include "bmp_factory/bmp_factory.h"

static struct stegobmp_args args;

int main(int argc, char *argv[]) {

    parse_args(argc, argv, &args);

    bmp_t *bmp = create_bmp(args.carrier);
    if (bmp == NULL) {
        return -1;
    }

    destroy_bmp(bmp);

    return 0;
}

