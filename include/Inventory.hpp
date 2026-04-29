#pragma once

#include "CardDatabase.hpp"

#include <string>
#include <unordered_map>
#include <vector>

struct InventoryEntry {
  std::string cardId;
  int count = 0;
};

class Inventory {
public:
  Inventory &operator+=(const std::string &cardId);

  void addCard(const std::string &cardId, int count = 1);
  bool removeCard(const std::string &cardId, int count = 1);
  int getCount(const std::string &cardId) const;
  int sellCard(const std::string &cardId, const CardDatabase &cardDatabase);
  int sellDuplicates(const CardDatabase &cardDatabase);
  int calculateTotalValue(const CardDatabase &cardDatabase) const;
  std::vector<InventoryEntry> getEntries() const;
  const std::unordered_map<std::string, int> &getCardCounts() const;
  void setCardCounts(const std::unordered_map<std::string, int> &newCardCounts);

private:
  int calculateTotalValueRecursive(const std::vector<InventoryEntry> &entries,
                                   const CardDatabase &cardDatabase,
                                   int index = 0) const;

  std::unordered_map<std::string, int> cardCounts;
};
