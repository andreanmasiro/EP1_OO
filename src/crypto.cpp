#include "crypto.hpp"

#include <cstdio>
#include <iostream>

#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/conf.h>

#include <iostream>

namespace crypto {
    
    
    array::array* sha1(const array::array* data) {
        array::array* hash = array::create(20);
        SHA1(data->data, data->length, hash->data);
        return hash;
    }
    
    array::array* nonce(size_t length) {
        array::array* n = array::create(length);
        RAND_bytes(n->data, length);
        return n;
    }
    
    RSA* rsa_read_public_key_from_PEM(const std::string& path) {
        std::cout << "(i) " << __func__ << ": trying to open " << path << std::endl;
        FILE* pem_file = fopen(path.c_str(), "rb");
        
        if(pem_file == nullptr || pem_file == 0) {
            std::cout << "(e) "<< __func__ << ": could not open file: " << path << std::endl;
            return nullptr;
        }
        
        RSA* rsa = PEM_read_RSA_PUBKEY(pem_file, NULL, NULL, NULL);
        
        fclose(pem_file);
        
        if(rsa == nullptr || rsa == 0) {
            std::cout << "(e) "<< __func__ << ": null key." << std::endl;
            return nullptr;
        }
        
        return rsa;
    }
    
    RSA* rsa_read_private_key_from_PEM(const std::string& path) {
        std::cout << "(i) " << __func__ << ": trying to open " << path << std::endl;
        FILE* pem_file = fopen(path.c_str(), "rb");
        
        if(pem_file == nullptr)
            return nullptr;
        
        RSA* rsa = PEM_read_RSAPrivateKey(pem_file, NULL,NULL, NULL);
        
        fclose(pem_file);
        
        return rsa;
    }
    
    void rsa_destroy_key(RSA* key) {
        std::cout << "(i) "<< __func__ << ": destroying RSA key." << std::endl;
        RSA_free(key);
    }
    
    array::array* rsa_encrypt(const array::array* data, RSA* rsa) {
        
        if(data == nullptr || data == 0) {
            std::cout << "(i) " << __func__ << ": empty or null data." << std::endl;
            return nullptr;
        }
        
        if(rsa == nullptr || rsa == 0) {
            std::cout << "(i)" << __func__ << ": empty or null key." << std::endl;
            return nullptr;
        }
        
        int result;
        byte* out = new byte[RSA_size(rsa)];
        
        result = RSA_public_encrypt(data->length, data->data, out, rsa, RSA_PKCS1_PADDING);
        
        if(result < 0) {
            delete[] out;
            return nullptr;
        }
        
        array::array* encrypted = array::create(result, out);
        delete[] out;
        
        return encrypted;
    }
    
    array::array* rsa_decrypt(const array::array* data, RSA* rsa) {
        if(data == nullptr || data == 0) {
            std::cout << __func__ << ": empty or null data." << std::endl;
            return nullptr;
        }
        if(rsa == nullptr || rsa == 0) {
            std::cout << __func__ << ": empty or null key." << std::endl;
            return nullptr;
        }
        int result;
        byte* out = new byte[RSA_size(rsa)];
        
        result = RSA_private_decrypt(data->length, data->data, out, rsa, RSA_PKCS1_PADDING);
        
        if(result < 0) {
            delete[] out;
            return nullptr;
        }
        array::array* decrypted = array::create(result, out);
        delete[] out;
        
        return decrypted;
    }
    
    array::array* aes_encrypt(const array::array* data, const array::array* iv, const array::array* key) {
        
        byte* ivec = new byte[iv->length];
        memcpy(ivec, iv->data, iv->length);
        byte* outdata = new byte[data->length * 2];
        
        int outLen1 = 0, outLen2 = 0;
        
        EVP_CIPHER_CTX ctx;
        EVP_EncryptInit(&ctx, EVP_aes_256_cbc(), key->data, ivec);
        EVP_EncryptUpdate(&ctx, outdata, &outLen1, data->data, data->length);
        EVP_EncryptFinal(&ctx,outdata + outLen1, &outLen2);
        
        delete[] ivec;
        
        array::array* lol = array::create(outLen1 + outLen2, outdata);
        
        delete[] outdata;
        
        return lol;
    }
    
    array::array* aes_decrypt(array::array* data, array::array* iv, array::array* key) {
        byte* ivec = new byte[iv->length];
        memcpy(ivec, iv->data, iv->length);
        byte* outdata = new byte[data->length * 2];
        
        int outLen1 = 0, outLen2 = 0;
        
        EVP_CIPHER_CTX ctx;
        EVP_DecryptInit(&ctx,EVP_aes_256_cbc(),key->data,ivec);
        EVP_DecryptUpdate(&ctx,outdata, &outLen1, data->data, data->length);
        EVP_DecryptFinal(&ctx,outdata + outLen1, &outLen2);
        
        delete[] ivec;
        
        array::array* lol = array::create(outLen1 + outLen2, outdata);
        
        delete[] outdata;
        
        return lol;
    }
    
}
