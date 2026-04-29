#include "Inventory.hpp"

#include <algorithm>

Inventory& Inventory::operator+=(const std::string& cardId)
{
    addCard(cardId);
    return *this;
}

void Inventory::addCard(const std::string& cardId, int count)
{
    if (count > 0)
    {
        cardCounts[cardId] += count;
    }
}

bool Inventory::removeCard(const std::string& cardId, int count)
{
    const auto cardIterator = cardCounts.find(cardId);
    if (cardIterator == cardCounts.end() || cardIterator->second < count || count <= 0)
    {
        return false;
    }

    cardIterator->second -= count;
    if (cardIterator->second == 0)
    {
        cardCounts.erase(cardIterator);
    }
    return true;
}

int Inventory::getCount(const std::string& cardId) const
{
    const auto cardIterator = cardCounts.find(cardId);
    return cardIterator == cardCounts.end() ? 0 : cardIterator->second;
}

int Inventory::sellCard(const std::string& cardId, const CardDatabase& cardDatabase)
{
    const Card* card = cardDatabase.findCard(cardId);
    if (card == nullptr || !removeCard(cardId))
    {
        return 0;
    }
    return card->getSellValue();
}

int Inventory::sellDuplicates(const CardDatabase& cardDatabase)
{
    int totalValue = 0;
    for (auto& cardPair : cardCounts)
    {
        const Card* card = cardDatabase.findCard(cardPair.first);
        if (card != nullptr && cardPair.second > 1)
        {
            const int duplicateCount = cardPair.second - 1;
            totalValue += duplicateCount * card->getSellValue();
            cardPair.second = 1;
        }
    }
    return totalValue;
}

int Inventory::calculateTotalValue(const CardDatabase& cardDatabase) const
{
    return calculateTotalValueRecursive(getEntries(), cardDatabase);
}

std::vector<InventoryEntry> Inventory::getEntries() const
{
    std::vector<InventoryEntry> entries;
    for (const auto& cardPair : cardCounts)
    {
        entries.push_back(InventoryEntry{cardPair.first, cardPair.second});
    }
    std::sort(entries.begin(), entries.end(), [](const InventoryEntry& leftEntry, const InventoryEntry& rightEntry) {
        return leftEntry.cardId < rightEntry.cardId;
    });
    return entries;
}

const std::unordered_map<std::string, int>& Inventory::getCardCounts() const
{
    return cardCounts;
}

void Inventory::setCardCounts(const std::unordered_map<std::string, int>& newCardCounts)
{
    cardCounts = newCardCounts;
}

int Inventory::calculateTotalValueRecursive(const std::vector<InventoryEntry>& entries, const CardDatabase& cardDatabase, int index) const
{
    if (index >= static_cast<int>(entries.size()))
    {
        return 0;
    }

    const Card* card = cardDatabase.findCard(entries[static_cast<size_t>(index)].cardId);
    const int currentValue = card == nullptr ? 0 : card->getSellValue() * entries[static_cast<size_t>(index)].count;
    return currentValue + calculateTotalValueRecursive(entries, cardDatabase, index + 1);
}
