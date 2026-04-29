#pragma once

#include "CardDatabase.hpp"
#include "GameState.hpp"
#include "PackDatabase.hpp"

#include <random>
#include <string>
#include <vector>

struct PackOpenResult
{
    std::string packName;
    std::vector<std::string> cardIds;
    int autoSoldCount = 0;
    int autoSoldValue = 0;
};

class PackOpeningService
{
public:
    static int totalPacksOpened;

    PackOpeningService();
    PackOpenResult openPack(const Pack& pack, GameState& gameState, const CardDatabase& cardDatabase);

private:
    Rarity rollRarity(const Pack& pack);
    const Card* rollCard(const Pack& pack, Rarity rarity, const CardDatabase& cardDatabase);
    bool shouldAutoSell(const Card& card, const AutoSellSettings& autoSellSettings) const;

    std::mt19937 randomEngine;
};
