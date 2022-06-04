#include "utils/args/args.h"
#include "utils/logger/logger.h"

#include <getopt.h>
#include <stdio.h>  /* for printf */
#include <stdlib.h> /* for exit */
#include <string.h> /* memset */

static const char *steg_strings[] = {
        "LSB1", "LSB4", "LSBI"
};

static const char *enc_strings[] = {
        "AES_128", "AES_192", "AES_256", "DES"
};

static const char *chain_mode_strings[] = {
        "ECB", "CFB", "OFB", "CBC"
};

const char *
steg_algorithm_string(int steg) {
    return steg_strings[steg];
}

const char *
enc_algorithm_string(int enc) {
    return enc_strings[enc];
}

const char *
chain_mode_string(int chain_mode) {
    return chain_mode_strings[chain_mode];
}

static enc_algorithm_t
get_enc(char *enc) {
    if (strcmp("aes128", enc) == 0) {
        return AES_128;
    } else if (strcmp("aes192", enc) == 0) {
        return AES_192;
    } else if (strcmp("aes256", enc) == 0) {
        return AES_256;
    } else if (strcmp("des", enc) == 0) {
        return DES;
    }

    return -1;
}

static chain_mode_t
get_mode(char *enc) {
    if (strcmp("ecb", enc) == 0) {
        return ECB;
    } else if (strcmp("cfb", enc) == 0) {
        return CFB;
    } else if (strcmp("ofb", enc) == 0) {
        return OFB;
    } else if (strcmp("cbc", enc) == 0) {
        return CBC;
    }

    return -1;
}

static steg_algorithm_t
get_steg(char *enc) {
    if (strcmp("LSB1", enc) == 0) {
        return LSB1;
    } else if (strcmp("LSB4", enc) == 0) {
        return LSB4;
    } else if (strcmp("LSBI", enc) == 0) {
        return LSBI;
    }
    return -1;
}

static void
version(void) {
    fprintf(stderr, "stegobmp version 1.0\n");
}

static void
usage(char *progname) {
    fprintf(stderr,
            "NAME\n"
            "\t%s – análisis y ocultación de información mediante esteganografía\n\n"
            "SYNOPSIS\n"
            "\t%s -embed -in file -p in_file -out out_file -steg steg_algorithm [-a enc_algorithm] [-m mode] [-pass password]\n\n"
            "\t%s -extract -p in_file -out out_file -steg steg_algorithm [-a enc_algorithm] [-m mode] [-pass password]\n\n"
            "DESCRIPTION\n"
            "\t%s permite ocultar un archivo cualquiera en un archivo .bmp, mediante un método de esteganografiado elegido, con o sin password.\n"
            "\tTambién permite estegoanaliczar un archivo .bmp para determinar si tiene un archivo incrustado, con qué algoritmo y lo extraiga correctamente.\n\n"
            "\t-embed                 Indica que se va a ocultar información.\n\n"
            "\t-extract               Indica que se va a extraer información.\n"
            "\t-in file               Archivo que se va a ocultar.\n\n"
            "\t-p bitmap_file         Archivo bmp que será el portador.\n\n"
            "\t-out bitmap_file       Archivo bmp de salida\n"
            "\t                       con la información de file incrustada.\n\n"
            "\t-steg steg_algorithm    Algoritmo de esteganografiado, valores posibles:\n"
            "\t                        -   LSB1: LSB de 1bit\n"
            "\t                        -   LSB4: LSB de 4 bits\n"
            "\t                        -   LSBI: LSB Enhanced\n"
            "\t-a enc_algorithm        Algoritmo de encripcion, valores posibles:\n"
            "\t                        -   aes128\n"
            "\t                        -   aes192\n"
            "\t                        -   aes256\n"
            "\t                        -   des\n"
            "\t-m mode                 Modo de encadenamiento, valores posibles:\n"
            "\t                        -   ecb\n"
            "\t                        -   cfb\n"
            "\t                        -   ofb\n"
            "\t                        -   cbc\n"
            "\t-pass password         Password de encripción\n\n",
            progname, progname, progname, progname);

    exit(1);
}
//FIXME: LOS ENUMS CUANDO PASO ALGO NO VALIDO EXPLOTA PORQUE -1 DA EL MAS CHICO
void parse_args(int argc, char **argv, stegobmp_args_t *args) {
    memset(args, 0, sizeof(*args));
    log_set_quiet(true);

    int c;

    while (true) {
        int option_index = 0;
        static struct option long_options[] = {
                {"embed",   no_argument,       0, 0xD001},
                {"in",      required_argument, 0, 0xD002},
                {"out",     required_argument, 0, 0xD003},
                {"steg",    required_argument, 0, 0xD004},
                {"pass",    required_argument, 0, 0xD005},
                {"extract", no_argument,       0, 0xD006},
                {"log",     no_argument,       0, 0xD007},
                {0, 0,                         0, 0}};

        c = getopt_long_only(argc, argv, "p:a:m:vh", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 'h':
                usage("stegobmp");
                break;
            case 'p':
                args->carrier = optarg;
                break;
            case 'a':
                args->enc = get_enc(optarg);
                break;
            case 'm':
                args->mode = get_mode(optarg);
                break;
            case 'v':
                version();
                exit(0);
            case 0xD001:
                args->embed = true;
                break;
            case 0xD002:
                args->in_file = optarg;
                break;
            case 0xD003:
                args->out_file = optarg;
                break;
            case 0xD004:
                args->steg = get_steg(optarg);
                break;
            case 0xD005:
                args->pass = optarg;
                break;
            case 0xD006:
                args->extract = true;
                break;
            case 0xD007:
                log_set_quiet(false);
                log_debug("logs enabled\n");
                break;
            default:
                fprintf(stderr, "unknown argument %d.\n", c);
                exit(1);
        }
    }

    if (optind < argc) {
        fprintf(stderr, "argument not accepted: ");
        while (optind < argc) {
            fprintf(stderr, "%s ", argv[optind++]);
        }
        fprintf(stderr, "\n");
        exit(1);
    }

    if (!args->embed && !args->extract) {
        fprintf(stderr, "either '-extract' or '-embed' arguments must be specified.\n");
        exit(1);
    }

    if (args->embed) {
        if (!args->in_file || !args->carrier || !args->out_file || args->steg < 0) {
            fprintf(stderr, "embed missing required arguments: ");
            if (!args->in_file) {
                fprintf(stderr, "-in ");
            }
            if (!args->carrier) {
                fprintf(stderr, "-p ");
            }
            if (!args->out_file) {
                fprintf(stderr, "-out ");
            }
            if (args->steg < 0) {
                fprintf(stderr, "-steg");
            }
            fprintf(stderr, "\n");
            exit(1);
        }
    } else {
        if (!args->carrier) {
            if (!args->in_file || !args->carrier || !args->out_file || args->steg < 0) {
                fprintf(stderr, "extract missing required arguments: ");
                if (!args->carrier) {
                    fprintf(stderr, "-p ");
                }
                if (!args->out_file) {
                    fprintf(stderr, "-out ");
                }
                if (args->steg < 0) {
                    fprintf(stderr, "-steg");
                }
                fprintf(stderr, "\n");
                exit(1);
            }
        }
    }

    log_debug("arguments parsed successfully\n"
              "arguments:\n"
              "\tembed: %s\n"
              "\textract: %s\n"
              "\tin_file: %s\n"
              "\tcarrier: %s\n"
              "\tout_file: %s\n"
              "\tsteg: %s\n"
              "\tenc: %s\n"
              "\tmode: %s\n"
              "\tpass: %s\n",
              args->embed ? "true" : "false",
              args->extract ? "true" : "false",
              args->in_file ? args->in_file : "None",
              args->carrier,
              args->out_file,
              steg_algorithm_string(args->steg),
              enc_algorithm_string(args->enc),
              chain_mode_strings[args->mode],
              args->pass ? args->pass : "None"
    );
}
