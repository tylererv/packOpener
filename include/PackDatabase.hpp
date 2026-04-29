#pragma once

#include "Pack.hpp"

#include <string>
#include <vector>

class PackDatabase
{
public:
    bool loadFromFile(const std::string& filePath);
    const Pack* findPack(const std::string& packId) const;
    const std::vector<Pack>& getPacks() const;
    bool isLoaded() const;

private:
    std::vector<Pack> packs;
};
