#ifndef _ARGS_H_kFlmYm1tW9p5npzDr2opQJ9jM8
#define _ARGS_H_kFlmYm1tW9p5npzDr2opQJ9jM8

#include <stdbool.h>

typedef enum steg {
    LSB1,
    LSB4,
    LSBI
} steg_t;

typedef enum enc {
    AES_128,
    AES_192,
    AES_256,
    DES,
} enc_t;

typedef enum chain_mode {
    ECB,
    CFB,
    OFB,
    CBC,
} chain_mode_t;

struct stegobmp_args {
    bool embed;
    bool extract;
    char *in_file;
    char *carrier;
    char *out_file;
    steg_t steg;
    enc_t enc;
    chain_mode_t mode;
    char *pass;
};

/**
 * Interpreta la linea de comandos (argc, argv) llenando
 * args con defaults o la seleccion humana. Puede cortar
 * la ejecucion.
 */
void
parse_args(int argc, char **argv, struct stegobmp_args *args);

const char *
steg_string(int steg);

const char *
enc_string(int enc);

const char *
chain_mode_string(int chain_mode);


#endif
