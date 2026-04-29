#include "Pack.hpp"

#include <sstream>
#include <utility>

using namespace std;

Pack::Pack()
    : id(""), name(""), price(0), cardCount(0), rarityOdds(),
      eligibleCardIds() {}

Pack::Pack(string id, string name, int price, int cardCount)
    : id(move(id)), name(move(name)), price(price), cardCount(cardCount),
      rarityOdds(), eligibleCardIds() {}

Pack::Pack(string id, string name, int price, int cardCount,
           unordered_map<Rarity, int> rarityOdds,
           vector<string> eligibleCardIds)
    : id(move(id)), name(move(name)), price(price), cardCount(cardCount),
      rarityOdds(move(rarityOdds)), eligibleCardIds(move(eligibleCardIds)) {}

const string &Pack::getId() const { return id; }
const string &Pack::getName() const { return name; }
int Pack::getPrice() const { return price; }
int Pack::getCardCount() const { return cardCount; }
const unordered_map<Rarity, int> &Pack::getRarityOdds() const {
  return rarityOdds;
}
const vector<string> &Pack::getEligibleCardIds() const {
  return eligibleCardIds;
}

string Pack::getOddsSummary() const {
  ostringstream outputStream;
  bool wroteEntry = false;
  for (int rarityIndex = 0; rarityIndex < rarityCount; ++rarityIndex) {
    const Rarity rarity = static_cast<Rarity>(rarityIndex);
    const auto oddsIterator = rarityOdds.find(rarity);
    if (oddsIterator != rarityOdds.end() && oddsIterator->second > 0) {
      if (wroteEntry)
        outputStream << " | ";
      outputStream << toString(rarity) << " " << oddsIterator->second;
      wroteEntry = true;
    }
  }
  return outputStream.str();
}
