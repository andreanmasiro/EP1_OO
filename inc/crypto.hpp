#ifndef _CRYPTO_H
#define _CRYPTO_H

#include "array.hpp"

#include <string>
#include <openssl/rsa.h>

namespace crypto {

/*
   Computes the SHA1 hash from data.
 */
array::array* sha1(const array::array* data);

/*
   Generates length random bytes.
 */
array::array* nonce(size_t length);

/*
   Read RSA public key from a PEM file.
 */
RSA* rsa_read_public_key_from_PEM(const std::string& path);

/*
   Read RSA private key from a PEM file.
 */
RSA* rsa_read_private_key_from_PEM(const std::string& path);

/*
   Releases RSA key from memory.
*/
void rsa_destroy_key(RSA* key);

/*
   Encrypts data using RSA public key.
 */
array::array* rsa_encrypt(const array::array* data, RSA* rsa);

/*
   Decrypts data using RSA private key.
 */
array::array* rsa_decrypt(const array::array* data, RSA* rsa);

/*
   Encrypts data using AES key.
 */
array::array* aes_encrypt(const array::array* data, const array::array* iv, const array::array* key);

/*
   Decrypts data using AES key.
 */
array::array* aes_decrypt(array::array* data, array::array* iv, array::array* key);

}

#endif
