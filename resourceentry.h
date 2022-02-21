#ifndef RESOURCEENTRY_H
#define RESOURCEENTRY_H
#include <stdint.h>
#include <string>

struct ResourceEntry {

    uint32_t path_name_offset;
    uint32_t entry_offset;
    uint32_t entry_length;
    bool hasReplacement = false;
    std::string path;
    ResourceEntry(uint32_t path_name_offset, uint32_t entry_offset, uint32_t entry_length, std::string path)
        : path_name_offset(path_name_offset)
        , entry_offset(entry_offset)
        , entry_length(entry_length)
        , path(std::move(path))
    {
    }
};

#endif // RESOURCEENTRY_H
