#include "PackOpeningService.hpp"

#include <algorithm>
#include <chrono>

int PackOpeningService::totalPacksOpened = 0;

PackOpeningService::PackOpeningService()
    : randomEngine(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()))
{
}

PackOpenResult PackOpeningService::openPack(const Pack& pack, GameState& gameState, const CardDatabase& cardDatabase)
{
    PackOpenResult result;
    result.packName = pack.getName();

    for (int cardIndex = 0; cardIndex < pack.getCardCount(); ++cardIndex)
    {
        const Rarity rarity = rollRarity(pack);
        const Card* card = rollCard(pack, rarity, cardDatabase);
        if (card == nullptr)
        {
            continue;
        }

        result.cardIds.push_back(card->getId());
        if (shouldAutoSell(*card, gameState.getAutoSellSettings()))
        {
            result.autoSoldCount += 1;
            result.autoSoldValue += card->getSellValue();
            gameState.addCash(card->getSellValue());
        }
        else
        {
            gameState.getInventory() += card->getId();
        }
    }

    ++totalPacksOpened;
    gameState.incrementPacksOpened();
    gameState.getPackHistory().addEntry(pack.getName());
    return result;
}

Rarity PackOpeningService::rollRarity(const Pack& pack)
{
    int totalWeight = 0;
    for (const auto& oddsPair : pack.getRarityOdds())
    {
        totalWeight += std::max(0, oddsPair.second);
    }

    if (totalWeight <= 0)
    {
        return Rarity::common;
    }

    std::uniform_int_distribution<int> weightDistribution(1, totalWeight);
    int rolledWeight = weightDistribution(randomEngine);

    for (int rarityIndex = 0; rarityIndex < rarityCount; ++rarityIndex)
    {
        const Rarity rarity = static_cast<Rarity>(rarityIndex);
        const auto oddsIterator = pack.getRarityOdds().find(rarity);
        if (oddsIterator == pack.getRarityOdds().end())
        {
            continue;
        }
        rolledWeight -= std::max(0, oddsIterator->second);
        if (rolledWeight <= 0)
        {
            return rarity;
        }
    }

    return Rarity::common;
}

const Card* PackOpeningService::rollCard(const Pack& pack, Rarity rarity, const CardDatabase& cardDatabase)
{
    std::vector<const Card*> candidates;
    for (const std::string& cardId : pack.getEligibleCardIds())
    {
        const Card* card = cardDatabase.findCard(cardId);
        if (card != nullptr && card->getRarity() == rarity)
        {
            candidates.push_back(card);
        }
    }

    if (candidates.empty())
    {
        for (const std::string& cardId : pack.getEligibleCardIds())
        {
            const Card* card = cardDatabase.findCard(cardId);
            if (card != nullptr)
            {
                candidates.push_back(card);
            }
        }
    }

    if (candidates.empty())
    {
        return nullptr;
    }

    std::uniform_int_distribution<int> cardDistribution(0, static_cast<int>(candidates.size()) - 1);
    return candidates[static_cast<size_t>(cardDistribution(randomEngine))];
}

bool PackOpeningService::shouldAutoSell(const Card& card, const AutoSellSettings& autoSellSettings) const
{
    if (!autoSellSettings.enabled)
    {
        return false;
    }

    if (!autoSellSettings.sellByRarity && !autoSellSettings.sellByValue)
    {
        return card.getRarity() == Rarity::common;
    }

    const bool rarityMatches = autoSellSettings.sellByRarity && static_cast<int>(card.getRarity()) <= static_cast<int>(autoSellSettings.maxRarity);
    const bool valueMatches = autoSellSettings.sellByValue && card.getSellValue() >= autoSellSettings.minValue && card.getSellValue() <= autoSellSettings.maxValue;
    return rarityMatches || valueMatches;
}
