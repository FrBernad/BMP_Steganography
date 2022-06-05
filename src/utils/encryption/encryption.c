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

    uint8_t *cipher_name_and_mode = calloc(strlen(mode) + strlen(ciphername) + 2, sizeof(uint8_t));
    strcpy((char *) cipher_name_and_mode, ciphername);
    strcat(strcat((char *) cipher_name_and_mode, "-"), mode);

    *ciphertext = calloc((int) ceil(((double) plaintext_len / 16) + 1) * 18, sizeof(uint8_t));
    if (*ciphertext == NULL) {
        log_error("memory error");
        return -1;
    }

    uint8_t key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
    if (generate_iv_and_key(password, cipher_name_and_mode, (uint8_t *) DEFAULT_DIGEST, key, iv) < 0) {
        return -1;
    }

    /* Create and initialise the context */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        log_error("encryption error");
        return -1;
    }

    /* Initialise the encryption operation. */
    if (EVP_EncryptInit_ex(ctx, EVP_get_cipherbyname((char *) ciphername), NULL, key, iv) != 1) {
        log_error("encryption error");
        return -1;
    }

    int len = 0;

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (EVP_EncryptUpdate(ctx, *ciphertext, &len, plaintext, plaintext_len) != 1) {
        log_error("encryption error");
        return -1;
    }
    int ciphertext_len = len;

    /*
     * Finalise the encryption. Normally ciphertext bytes may be written at
     * this stage
     */
    if (EVP_EncryptFinal_ex(ctx, *ciphertext + len, &len) != 1) {
        log_error("encryption error");
        return -1;
    }
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int
decrypt(uint8_t *ciphertext, int ciphertext_len, uint8_t *password,
        char *ciphername, char *mode, uint8_t **plaintext) {

    uint8_t *cipher_name_and_mode = calloc(strlen(mode) + strlen(ciphername) + 2, sizeof(uint8_t));
    strcpy((char *) cipher_name_and_mode, ciphername);
    strcat(strcat((char *) cipher_name_and_mode, "-"), mode);

    *plaintext = calloc(ciphertext_len, sizeof(uint8_t));
    if (*plaintext == NULL) {
        log_error("memory error");
        return -1;
    }

    uint8_t key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
    if (generate_iv_and_key(password, cipher_name_and_mode, (uint8_t *) DEFAULT_DIGEST, key, iv) < 0) {
        return -1;
    }

    /* Create and initialise the context */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return -1;
    }

    /* Initialise the decryption operation. */
    if (!EVP_DecryptInit_ex(ctx, EVP_get_cipherbyname((char *) ciphername), NULL, key, iv)) {
        return -1;
    }

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    int len;
    int ret;
    if (!EVP_DecryptUpdate(ctx, *plaintext, &len, ciphertext, ciphertext_len)) {
        return -1;
    }
    int plaintext_len = len;

    /*
     * Finalise the decryption. A positive return value indicates success,
     * anything else is a failure - the plaintext is not trustworthy.
     */
    ret = EVP_DecryptFinal_ex(ctx, *plaintext + plaintext_len, &len);

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    if (ret > 0) {
        /* Success */
        plaintext_len += len;
        return plaintext_len;
    } else {
        /* Verify failed */
        return -1;
    }
}

static int
generate_iv_and_key(uint8_t *password, uint8_t *ciphername, uint8_t *digest_name, uint8_t *key, uint8_t *iv) {

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