#include "CardDatabase.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

static vector<string> splitLine(const string &line, char delimiter) {
  vector<string> parts;
  string part;
  stringstream lineStream(line);
  while (getline(lineStream, part, delimiter)) {
    parts.push_back(part);
  }
  return parts;
}

bool CardDatabase::loadFromFile(const string &filePath) {
  cards.clear();
  cardIndex.clear();

  ifstream inputFile(filePath);
  if (!inputFile) {
    cerr << "Could not open card data: " << filePath << '\n';
    return false;
  }

  string line;
  int lineNumber = 0;
  while (getline(inputFile, line)) {
    ++lineNumber;
    if (line.empty() || line[0] == '#') {
      continue;
    }

    try {
      const vector<string> fields = splitLine(line, '|');
      if (fields.size() < 7) {
        cerr << "Skipping malformed card row " << lineNumber << '\n';
        continue;
      }

      const string id = fields[0];
      const string name = fields[1];
      const Rarity rarity = rarityFromString(fields[2]);
      const CardType cardType = cardTypeFromString(fields[3]);
      const int sellValue = stoi(fields[4]);
      const string description = fields[5];
      const int powerValue = stoi(fields[6]);
      const string imagePath = fields.size() > 7 ? fields[7] : "";

      if (sellValue <= 0) {
        cerr << "Skipping card with non-positive value: " << id << '\n';
        continue;
      }

      unique_ptr<Card> card;
      if (cardType == CardType::spell) {
        card = make_unique<SpellCard>(id, name, rarity, sellValue, description,
                                      powerValue, imagePath);
      } else if (cardType == CardType::relic) {
        card = make_unique<RelicCard>(id, name, rarity, sellValue, description,
                                      powerValue, imagePath);
      } else {
        card = make_unique<CreatureCard>(id, name, rarity, sellValue,
                                         description, powerValue, imagePath);
      }

      cardIndex[id] = card.get();
      cards.push_back(move(card));
    } catch (const exception &error) {
      cerr << "Skipping card row " << lineNumber << ": " << error.what()
           << '\n';
    }
  }

  return !cards.empty();
}

const Card *CardDatabase::findCard(const string &cardId) const {
  const auto cardIterator = cardIndex.find(cardId);
  return cardIterator == cardIndex.end() ? nullptr : cardIterator->second;
}

vector<const Card *> CardDatabase::findCardsByRarity(Rarity rarity) const {
  vector<const Card *> matchingCards;
  for (const auto &card : cards) {
    if (card->getRarity() == rarity) {
      matchingCards.push_back(card.get());
    }
  }
  return matchingCards;
}

const vector<unique_ptr<Card>> &CardDatabase::getCards() const { return cards; }

bool CardDatabase::isLoaded() const { return !cards.empty(); }
