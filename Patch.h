#ifndef PATCH_H
#define PATCH_H
#include <stdint.h>
#include <string>

struct Patch {

    std::string path;

    Patch()
    {
    }

    Patch(std::string path)
        : path(std::move(path))
    {
    }
};
#endif // PATCH_H
