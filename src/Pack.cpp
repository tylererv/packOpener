#include "Pack.hpp"

#include <sstream>
#include <utility>

Pack::Pack()
    : id(""), name(""), price(0), cardCount(0), rarityOdds(), eligibleCardIds()
{
}

Pack::Pack(std::string id, std::string name, int price, int cardCount)
    : id(std::move(id)), name(std::move(name)), price(price), cardCount(cardCount), rarityOdds(), eligibleCardIds()
{
}

Pack::Pack(std::string id, std::string name, int price, int cardCount, std::unordered_map<Rarity, int> rarityOdds, std::vector<std::string> eligibleCardIds)
    : id(std::move(id)), name(std::move(name)), price(price), cardCount(cardCount), rarityOdds(std::move(rarityOdds)), eligibleCardIds(std::move(eligibleCardIds))
{
}

const std::string& Pack::getId() const { return id; }
const std::string& Pack::getName() const { return name; }
int Pack::getPrice() const { return price; }
int Pack::getCardCount() const { return cardCount; }
const std::unordered_map<Rarity, int>& Pack::getRarityOdds() const { return rarityOdds; }
const std::vector<std::string>& Pack::getEligibleCardIds() const { return eligibleCardIds; }

std::string Pack::getOddsSummary() const
{
    std::ostringstream outputStream;
    bool wroteEntry = false;
    for (int rarityIndex = 0; rarityIndex < rarityCount; ++rarityIndex)
    {
        const Rarity rarity = static_cast<Rarity>(rarityIndex);
        const auto oddsIterator = rarityOdds.find(rarity);
        if (oddsIterator != rarityOdds.end() && oddsIterator->second > 0)
        {
            if (wroteEntry) outputStream << " | ";
            outputStream << toString(rarity) << " " << oddsIterator->second;
            wroteEntry = true;
        }
    }
    return outputStream.str();
}
