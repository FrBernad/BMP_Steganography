#include <stdio.h>
#include "utils/I_O/I_O.h"
#include "utils/args/args.h"
#include "bmp_factory/bmp_factory.h"
#include "algorithms/algorithms.h"

static stegobmp_args_t args;

int main(int argc, char *argv[]) {

    parse_args(argc, argv, &args);

    bmp_t *bmp = create_bmp(args.carrier);
    if (bmp == NULL) {
        return -1;
    }

    I_O_resources_t resources;
    open_I_O_resources(&resources, args);

//    if (args.embed) {
//        if (embed(args.steg, bmp, resources) < 0) {
//            return -1;
//        }
//    } else {
//        if (extract(args.steg, bmp, resources) < 0) {
//            return -1;
//        }
//    }

    free_bmp(bmp);
    close_I_O_resources(&resources);

    return 0;
}

