#ifndef RESOURCEBIN_H
#define RESOURCEBIN_H

#include "resourceentry.h"

#include "Patch.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>

class ResourceBin {
private:
    std::string path;
    std::ifstream stream;
    int64_t length;
    uint32_t offset_header;
    uint32_t compressed_length_header;
    int32_t uncompressed_length_header;
    std::vector<ResourceEntry> entries;
    bool closed = false;

    void load_header();
    void decode(int offset, int length, char* in_buffer);
    void decode_input(int offset, int length, char* buffer);
    int gzip_uncompress(char* compressed, int compressed_length, char* uncompressed, int* uncompressed_length);
    int gzip_compress(char* uncompressed, int uncompressed_length, char* compressed, int* compressed_length);

    void init_gzip_stream();
    int gzip_compress_inplace(unsigned char* buf, unsigned len, unsigned* max);

public:
    ResourceBin(const std::string& path);
    ~ResourceBin();
    void init();
    void close();
    const std::string& get_path()
    {
        return path;
    }
    std::vector<char> extract(const ResourceEntry& header_entry);
    std::vector<ResourceEntry>& getEntries();

    //void create_with_modifications(std::unordered_map<std::string, Patch>& patchMap, std::string path);

    void create_with_modifications(std::unordered_map<std::string, Patch>& patchMap, const std::string& path, const std::function<void(int)>& callback);
};

#endif // RESOURCEBIN_H
