#ifndef _ARGS_H_
#define _ARGS_H_

#include <stdbool.h>

typedef enum steg_algorithm {
    LSB1,
    LSB4,
    LSBI
} steg_algorithm_t;

typedef enum enc_algorithm {
    AES_128,
    AES_192,
    AES_256,
    DES,
} enc_algorithm_t;

typedef enum chain_mode {
    ECB,
    CFB,
    OFB,
    CBC,
} chain_mode_t;

typedef struct stegobmp_args {
    bool embed;
    bool extract;
    char *in_file;
    char *carrier;
    char *out_file;
    steg_algorithm_t steg;
    enc_algorithm_t enc;
    chain_mode_t mode;
    char *pass;
} stegobmp_args_t;

/**
 * Interpreta la linea de comandos (argc, argv) llenando
 * args con defaults o la seleccion humana. Puede cortar
 * la ejecucion.
 */
void
parse_args(int argc, char **argv, stegobmp_args_t *args);

const char *
steg_algorithm_string(int steg);

const char *
enc_algorithm_string(int enc);

const char *
chain_mode_string(int chain_mode);


#endif
