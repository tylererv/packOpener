#include "CardDatabase.hpp"

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
}

bool CardDatabase::loadFromFile(const std::string& filePath)
{
    cards.clear();
    cardIndex.clear();

    std::ifstream inputFile(filePath);
    if (!inputFile)
    {
        std::cerr << "Could not open card data: " << filePath << '\n';
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
            if (fields.size() < 7)
            {
                std::cerr << "Skipping malformed card row " << lineNumber << '\n';
                continue;
            }

            const std::string id = fields[0];
            const std::string name = fields[1];
            const Rarity rarity = rarityFromString(fields[2]);
            const CardType cardType = cardTypeFromString(fields[3]);
            const int sellValue = std::stoi(fields[4]);
            const std::string description = fields[5];
            const int powerValue = std::stoi(fields[6]);
            const std::string imagePath = fields.size() > 7 ? fields[7] : "";

            if (sellValue <= 0)
            {
                std::cerr << "Skipping card with non-positive value: " << id << '\n';
                continue;
            }

            std::unique_ptr<Card> card;
            if (cardType == CardType::spell)
            {
                card = std::make_unique<SpellCard>(id, name, rarity, sellValue, description, powerValue, imagePath);
            }
            else if (cardType == CardType::relic)
            {
                card = std::make_unique<RelicCard>(id, name, rarity, sellValue, description, powerValue, imagePath);
            }
            else
            {
                card = std::make_unique<CreatureCard>(id, name, rarity, sellValue, description, powerValue, imagePath);
            }

            cardIndex[id] = card.get();
            cards.push_back(std::move(card));
        }
        catch (const std::exception& error)
        {
            std::cerr << "Skipping card row " << lineNumber << ": " << error.what() << '\n';
        }
    }

    return !cards.empty();
}

const Card* CardDatabase::findCard(const std::string& cardId) const
{
    const auto cardIterator = cardIndex.find(cardId);
    return cardIterator == cardIndex.end() ? nullptr : cardIterator->second;
}

std::vector<const Card*> CardDatabase::findCardsByRarity(Rarity rarity) const
{
    std::vector<const Card*> matchingCards;
    for (const auto& card : cards)
    {
        if (card->getRarity() == rarity)
        {
            matchingCards.push_back(card.get());
        }
    }
    return matchingCards;
}

const std::vector<std::unique_ptr<Card>>& CardDatabase::getCards() const
{
    return cards;
}

bool CardDatabase::isLoaded() const
{
    return !cards.empty();
}
