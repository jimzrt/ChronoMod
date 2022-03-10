#include "ChronoCrypto.h"
#include <cstring>
#include <vector>

void encrypt(const uint32_t* key_buffer, uint32_t* encrypted_1, uint32_t* encrypted_2)
{
    uint32_t tmp1 = 0;
    uint32_t tmp2 = *encrypted_1;
    uint32_t tmp3 = *encrypted_2;
    for (int i = 0; i < 0x10; ++i) {
        tmp1 = tmp2 ^ key_buffer[i];
        tmp2 = (key_buffer[(tmp1 >> 0x18) + 0x12] + key_buffer[(tmp1 >> 0x10 & 0xff) + 0x112] ^ key_buffer[(tmp1 >> 8 & 0xff) + 0x212]) + key_buffer[(tmp1 & 0xff) + 0x312] ^ tmp3;
        tmp3 = tmp1;
    }
    *encrypted_1 = key_buffer[0x11] ^ tmp1;
    *encrypted_2 = key_buffer[0x10] ^ tmp2;
}

void decrypt(const uint32_t* key_buffer, uint32_t* decrypted_1, uint32_t* decrypted_2)
{
    uint32_t tmp1 = 0;
    uint32_t tmp2 = *decrypted_1;
    uint32_t tmp3 = *decrypted_2;
    for (int i = 0; i < 0x10; ++i) {
        tmp1 = tmp2 ^ key_buffer[0x11 - i];
        tmp2 = (key_buffer[(tmp1 >> 0x18) + 0x12] + key_buffer[(tmp1 >> 0x10 & 0xff) + 0x112] ^ key_buffer[(tmp1 >> 8 & 0xff) + 0x212]) + key_buffer[(tmp1 & 0xff) + 0x312] ^ tmp3;
        tmp3 = tmp1;
    }
    *decrypted_1 = *key_buffer ^ tmp1;
    *decrypted_2 = key_buffer[1] ^ tmp2;
}

void crypt(uint32_t* key_buffer, int should_encrypt, const uint8_t* crypt_in, uint8_t* crypt_out)
{
    uint32_t crypt_in_1 = ((uint32_t)crypt_in[0] << 0x18) + ((uint32_t)crypt_in[1] << 0x10) + ((uint32_t)crypt_in[2] << 0x8) + ((uint32_t)crypt_in[3]);
    uint32_t crypt_in_2 = ((uint32_t)crypt_in[4] << 0x18) + ((uint32_t)crypt_in[5] << 0x10) + ((uint32_t)crypt_in[6] << 0x8) + ((uint32_t)crypt_in[7]);

    if (should_encrypt == 0) {
        decrypt(key_buffer, &crypt_in_1, &crypt_in_2);
    } else {
        encrypt(key_buffer, &crypt_in_1, &crypt_in_2);
    }
    crypt_out[0] = (uint8_t)(crypt_in_1 >> 0x18);
    crypt_out[1] = (uint8_t)(crypt_in_1 >> 0x10);
    crypt_out[2] = (uint8_t)(crypt_in_1 >> 8);
    crypt_out[3] = (uint8_t)crypt_in_1;
    crypt_out[4] = (uint8_t)(crypt_in_2 >> 0x18);
    crypt_out[5] = (uint8_t)(crypt_in_2 >> 0x10);
    crypt_out[6] = (uint8_t)(crypt_in_2 >> 8);
    crypt_out[7] = (uint8_t)crypt_in_2;
}

void crypto(uint32_t* key_buffer, int should_encrypt, size_t buffer_size, uint8_t* header, uint8_t* encData)
{
    uint8_t out_buffer[8];
    int buffer_index = 0;
    buffer_size = buffer_size / 8;
    for (size_t i = 0; i < buffer_size; ++i) {
        uint8_t* buffer_chunk = encData + buffer_index;
        if (should_encrypt == 0) {
            crypt(key_buffer, 0, buffer_chunk, out_buffer);
            for (int j = 0; j < 8; ++j) {
                out_buffer[j] = out_buffer[j] ^ header[j];
            }
            *(uint64_t*)header = *(uint64_t*)buffer_chunk;
            *(uint64_t*)buffer_chunk = *(uint64_t*)out_buffer;
        } else {
            for (int j = 0; j < 8; ++j) {
                out_buffer[j] = buffer_chunk[j] ^ header[j];
            }
            crypt(key_buffer, should_encrypt, out_buffer, out_buffer);
            *(uint64_t*)header = *(uint64_t*)out_buffer;
            *(uint64_t*)buffer_chunk = *(uint64_t*)out_buffer;
        }
        buffer_index += 8;
    }
}

void header_magic(char* header)
{
    header[0] ^= 0x75;
    header[1] ^= 0xFA;
    header[2] ^= 0x29;
    header[3] ^= 0x95;
    header[4] ^= 0x05;
    header[5] ^= 0x4D;
    header[6] ^= 0x41;
    header[7] ^= 0x5F;
}

void blowfish(uint32_t* keybuffer, char* blowfish_s_init, const char* blowfish_p_init, const char* blowfish_key)
{

    memcpy(keybuffer + 18, blowfish_s_init, 0x1000);

    int crypt = 0;
    for (int step = 0; step < 18; step++) {
        int crypt_xor = (uint8_t)blowfish_key[(crypt + 3) % 8] | (((uint8_t)blowfish_key[(crypt + 2) % 8] | (((uint8_t)blowfish_key[(crypt + 1) % 8] | ((uint8_t)blowfish_key[crypt] << 8)) << 8)) << 8);
        *(keybuffer + step) = crypt_xor ^ *((int*)(blowfish_p_init) + step);
        crypt = (crypt + 4) % 8;
    }

    uint32_t crypt1 = 0, crypt2 = 0;
    for (int step = 0; step < 18; step += 2) {
        encrypt(keybuffer, &crypt1, &crypt2);
        *(keybuffer + step) = crypt1;
        *(keybuffer + step + 1) = crypt2;
    }
    uint32_t* currentKeybuffer = keybuffer + 18;
    for (int i = 0; i < 512; i++) {
        encrypt(keybuffer, &crypt1, &crypt2);
        *currentKeybuffer = crypt1;
        *(currentKeybuffer + 1) = crypt2;
        currentKeybuffer += 2;
    }
}

void blowfish_chrono(uint32_t* keybuffer, char* chrono_binary)
{
    char* blowfish_s_init = chrono_binary + 0x39A138;
    char* blowfish_p_init = blowfish_s_init + 0x1000;
    char* blowfish_key = blowfish_s_init - 0x1850; // 0xba, 0xec, 0xb7, 0x8a, 0xea, 0x25, 0xca, 0xe4
    blowfish(keybuffer, blowfish_s_init, blowfish_p_init, blowfish_key);
}

std::vector<uint32_t> get_key(char* chrono_binary)
{
    std::vector<uint32_t> key(1043);
    blowfish_chrono(key.data(), chrono_binary);
    return key;
}

char* decrypt_file_with_binary(char* chrono_binary, char* encrypted, int encrypted_length)
{
    uint32_t key[1043];
    memset(key, 0, 0x1048);
    blowfish_chrono(key, chrono_binary);

    header_magic(encrypted);
    char* header = encrypted;
    char* data = encrypted + 8;

    crypto(key, 0, encrypted_length - 8, (uint8_t*)header, (uint8_t*)data);

    return data;
}

std::vector<char> decrypt_file_with_key(uint32_t* key, char* encrypted, int encrypted_length)
{
    header_magic(encrypted);
    char* header = encrypted;
    char* data = encrypted + 8;

    crypto(key, 0, encrypted_length - 8, (uint8_t*)header, (uint8_t*)data);

    return { data, data + encrypted_length - 8 };
}

std::vector<char> encrypt_file_with_key(uint32_t* key, char* decrypted, int decrypted_length)
{
    char header[8] = { 0 };
    header_magic(header);
    crypto(key, 1, decrypted_length, (uint8_t*)header, (uint8_t*)decrypted);
    std::vector<char> encrypted(decrypted_length + 8);
    memset(encrypted.data(), 0, 8);
    memcpy(encrypted.data() + 8, decrypted, decrypted_length);
    return encrypted;
}
