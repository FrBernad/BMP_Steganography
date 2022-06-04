#include <openssl/evp.h>
#include <string.h>
#include <math.h>
#include "encryption.h"
#include "utils/logger/logger.h"

#define DEFAULT_DIGEST "sha256"

static int
generate_iv_and_key(uint8_t *password, uint8_t *ciphername, uint8_t *digest_name, uint8_t *key, uint8_t *iv);

int
encrypt_data(uint8_t *plaintext, int plaintext_len, uint8_t *password,
             char *ciphername, char *mode, uint8_t **ciphertext) {

    EVP_CIPHER_CTX *ctx;

    uint8_t *cipher_name_and_mode = (uint8_t *) strcat(strcat(ciphername, "-"), mode);

    int len;

    int ciphertext_len;

    ciphertext = calloc((int) ceil(((double) plaintext_len / 16) + 1) * 16, sizeof(uint8_t));

    uint8_t key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];

    if (generate_iv_and_key(password, cipher_name_and_mode, (uint8_t *) DEFAULT_DIGEST, key, iv) < 0) {
        return -1;
    }

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        log_error("encryption error");
        return -1;
    }

    /* Initialise the encryption operation. */
    if (1 != EVP_EncryptInit_ex(ctx, EVP_get_cipherbyname((char *) ciphername), NULL, key, iv)) {
        log_error("encryption error");
        return -1;
    }

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (1 != EVP_EncryptUpdate(ctx, *ciphertext, &len, plaintext, plaintext_len)) {
        log_error("encryption error");
        return -1;
    }
    ciphertext_len = len;

    /*
     * Finalise the encryption. Normally ciphertext bytes may be written at
     * this stage, but this does not occur in GCM mode
     */
    if (1 != EVP_EncryptFinal_ex(ctx, *ciphertext + len, &len)) {
        log_error("encryption error");
        return -1;
    }
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

static int
generate_iv_and_key(uint8_t *password, uint8_t *ciphername, uint8_t *digest_name, uint8_t *key, uint8_t *iv) {

    OpenSSL_add_all_algorithms();

    const EVP_CIPHER *cipher = EVP_get_cipherbyname((char *) ciphername);
    if (!cipher) {
        log_error("no such cipher %s\n", ciphername);
        return -1;
    }

    const EVP_MD *dgst = EVP_get_digestbyname((char *) digest_name);
    if (!dgst) {
        log_error("no such digest %s\n", digest_name);
        return -1;
    }

    const uint8_t *salt = NULL;
    if (!EVP_BytesToKey(cipher, dgst, salt,
                        (uint8_t *) password,
                        (int) strlen((char *) password), 1, key, iv)) {
        log_error("EVP_BytesToKey failed\n");
        return -1;
    }

    return 1;
}