#include "PackDatabase.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace
{
    std::vector<std::string> splitLine(const std::string& line, char delimiter)
    {
        std::vector<std::string> parts;
        std::string part;
        std::stringstream lineStream(line);
        while (std::getline(lineStream, part, delimiter))
        {
            parts.push_back(part);
        }
        return parts;
    }

    std::unordered_map<Rarity, int> parseOdds(const std::string& oddsText)
    {
        std::unordered_map<Rarity, int> rarityOdds;
        const std::vector<std::string> oddsParts = splitLine(oddsText, ',');
        for (const std::string& oddsPart : oddsParts)
        {
            const std::vector<std::string> keyValue = splitLine(oddsPart, ':');
            if (keyValue.size() == 2)
            {
                rarityOdds[rarityFromString(keyValue[0])] = std::stoi(keyValue[1]);
            }
        }
        return rarityOdds;
    }
}

bool PackDatabase::loadFromFile(const std::string& filePath)
{
    packs.clear();

    std::ifstream inputFile(filePath);
    if (!inputFile)
    {
        std::cerr << "Could not open pack data: " << filePath << '\n';
        return false;
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(inputFile, line))
    {
        ++lineNumber;
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        try
        {
            const std::vector<std::string> fields = splitLine(line, '|');
            if (fields.size() < 6)
            {
                std::cerr << "Skipping malformed pack row " << lineNumber << '\n';
                continue;
            }

            packs.emplace_back(
                fields[0],
                fields[1],
                std::stoi(fields[2]),
                std::stoi(fields[3]),
                parseOdds(fields[4]),
                splitLine(fields[5], ',')
            );
        }
        catch (const std::exception& error)
        {
            std::cerr << "Skipping pack row " << lineNumber << ": " << error.what() << '\n';
        }
    }

    return !packs.empty();
}

const Pack* PackDatabase::findPack(const std::string& packId) const
{
    for (const Pack& pack : packs)
    {
        if (pack.getId() == packId)
        {
            return &pack;
        }
    }
    return nullptr;
}

const std::vector<Pack>& PackDatabase::getPacks() const
{
    return packs;
}

bool PackDatabase::isLoaded() const
{
    return !packs.empty();
}
