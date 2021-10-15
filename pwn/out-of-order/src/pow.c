#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "openssl/sha.h"

int main() {
    char input[256];
    gets(input);

    int nonce_len = strlen(input);

    for (uint64_t i = 0; i < 0xFFFFFFFFFFFFFFFF; i++) {
        sprintf(input+nonce_len, "%lu", i);

        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, input, strlen(input));
        SHA256_Final(hash, &sha256);

        if (hash[0] == 0 && hash[1] == 0 && hash[2] == 0) {
            fputs(input+nonce_len, stdout);
            fflush(stdout);
            exit(0);
        }
    }
    abort();
}

