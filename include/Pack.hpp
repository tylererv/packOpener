#pragma once

#include "Enums.hpp"

#include <string>
#include <unordered_map>
#include <vector>

class Pack
{
public:
    Pack();
    Pack(std::string id, std::string name, int price, int cardCount);
    Pack(std::string id, std::string name, int price, int cardCount, std::unordered_map<Rarity, int> rarityOdds, std::vector<std::string> eligibleCardIds);

    const std::string& getId() const;
    const std::string& getName() const;
    int getPrice() const;
    int getCardCount() const;
    const std::unordered_map<Rarity, int>& getRarityOdds() const;
    const std::vector<std::string>& getEligibleCardIds() const;
    std::string getOddsSummary() const;

private:
    std::string id;
    std::string name;
    int price;
    int cardCount;
    std::unordered_map<Rarity, int> rarityOdds;
    std::vector<std::string> eligibleCardIds;
};
