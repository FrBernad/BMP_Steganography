#ifndef _ENCRYPTION_H
#define _ENCRYPTION_H

int
encrypt_data(uint8_t *plaintext, int plaintext_len, uint8_t *password,
             char *ciphername, char *mode, uint8_t **ciphertext);

int
decrypt(uint8_t *ciphertext, int ciphertext_len, uint8_t *password,
        char *ciphername, char *mode, uint8_t **plaintext);

#endif
