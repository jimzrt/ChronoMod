#pragma once
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

void blowfish_chrono(uint32_t* keybuffer, char* chrono_binary);

void header_magic(char* header);

void crypto(uint32_t* key_buffer, int should_encrypt, size_t buffer_size, uint8_t* header, uint8_t* encData);

char* decrypt_file_with_binary(char* chrono_binary, char* encrypted, int encrypted_length);

std::vector<char> decrypt_file_with_key(uint32_t* key, char* encrypted, int encrypted_length);
std::vector<char> encrypt_file_with_key(uint32_t* key, char* decrypted, int decrypted_length);

std::vector<uint32_t> get_key(char* chrono_binary);
