#pragma once

#include "Enums.hpp"
#include "Inventory.hpp"
#include "PackHistoryList.hpp"

#include <string>

struct AutoSellSettings {
  bool enabled = false;
  bool sellByRarity = false;
  bool sellByValue = false;
  Rarity maxRarity = Rarity::common;
  int minValue = 0;
  int maxValue = 25;
};

class GameState {
public:
  GameState();

  int getCash() const;
  void addCash(int amount);
  bool spendCash(int amount);

  Inventory &getInventory();
  const Inventory &getInventory() const;
  AutoSellSettings &getAutoSellSettings();
  const AutoSellSettings &getAutoSellSettings() const;

  int getPacksOpened() const;
  void incrementPacksOpened();
  PackHistoryList &getPackHistory();
  const PackHistoryList &getPackHistory() const;

private:
  int cash;
  int packsOpened;
  Inventory inventory;
  AutoSellSettings autoSellSettings;
  PackHistoryList packHistory;
};
