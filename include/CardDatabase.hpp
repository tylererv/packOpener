#pragma once

#include "Card.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class CardDatabase {
public:
  bool loadFromFile(const std::string &filePath);
  const Card *findCard(const std::string &cardId) const;
  std::vector<const Card *> findCardsByRarity(Rarity rarity) const;
  const std::vector<std::unique_ptr<Card>> &getCards() const;
  bool isLoaded() const;

private:
  std::vector<std::unique_ptr<Card>> cards;
  std::unordered_map<std::string, const Card *> cardIndex;
};
