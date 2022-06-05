#include <stdio.h>
#include "utils/I_O/I_O.h"
#include "utils/args/args.h"
#include "bmp_factory/bmp_factory.h"
#include "algorithms/algorithms.h"

static stegobmp_args_t args;

int main(int argc, char *argv[]) {

    parse_args(argc, argv, &args);

    int return_val = 0;

    I_O_resources_t *resources = NULL;

    bmp_t *bmp = create_bmp(args.carrier);
    if (bmp == NULL) {
        return_val = -1;
        goto finally;
    }
    //FIXME: OJO OFFSET DEL BMP
    resources = open_I_O_resources(args);
    if (resources == NULL) {
        return_val = -1;
        goto finally;
    }

    if (args.embed) {
        if (embed(args.steg, bmp, resources) < 0) {
            return_val = -1;
            goto finally;
        }
        if (generate_embedded_bmp(bmp, resources) < 0) {
            return_val = -1;
            goto finally;
        }
    } else {
        if (extract(args.steg, bmp, resources, args.enc > 0) < 0) {
            return_val = -1;
            goto finally;
        }
        if (generate_extracted_file(resources->extracted_data, args) < 0) {
            return_val = -1;
            goto finally;
        }
    }

    finally:

    free_bmp(bmp);
    close_I_O_resources(resources);

    return return_val;
}

