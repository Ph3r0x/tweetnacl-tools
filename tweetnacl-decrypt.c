#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "randombytes.h"
#include "tools.h"
#include "tweetnacl.h"

int main(int argc, char *argv[]) {
    if (argc != 5) error(2, "Expected %s filenames as arguments", "4");

    // This will also erroneously fail if the file "-" exists
    if (file_exists(argv[4])) error(1, "File <%s> exists", argv[4]);

    // Alice has sent to Bob, not surprisingly
    unsigned char a_public_key[crypto_box_PUBLICKEYBYTES];
    unsigned char b_secret_key[crypto_box_SECRETKEYBYTES];

    read_key(argv[1], a_public_key, crypto_box_PUBLICKEYBYTES);
    read_key(argv[2], b_secret_key, crypto_box_SECRETKEYBYTES);

    unsigned char nonce[crypto_box_NONCEBYTES];

    // Input
    Content c = read_file(argv[3]);
    memcpy(nonce, c.bytes, crypto_box_NONCEBYTES);

    long esize = c.size - crypto_box_NONCEBYTES + crypto_box_BOXZEROBYTES;
    unsigned char *encrypted = malloc(esize);
    memset(encrypted, 0, crypto_box_BOXZEROBYTES);
    memcpy(encrypted + crypto_box_BOXZEROBYTES,
        c.bytes + crypto_box_NONCEBYTES, esize);

    // Output
    unsigned char *message = malloc(esize);
    memset(message, 0, esize);

    // Encrypt
    crypto_box_open(message, encrypted, esize,
        nonce, a_public_key, b_secret_key);

    if (strcmp(argv[4], "-") != 0) {
        FILE *out = create_file(argv[4]);
        fwrite(message + crypto_box_ZEROBYTES,
               esize - crypto_box_ZEROBYTES, 1, out);
        fclose(out);
    } else {
        fwrite(message + crypto_box_ZEROBYTES,
               esize - crypto_box_ZEROBYTES, 1, stdout);
    }
}