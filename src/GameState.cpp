#include "GameState.hpp"

GameState::GameState()
    : cash(0), packsOpened(0), inventory(), autoSellSettings(), packHistory() {}

int GameState::getCash() const { return cash; }

void GameState::addCash(int amount) {
  if (amount > 0) {
    cash += amount;
  }
}

bool GameState::spendCash(int amount) {
  if (amount < 0 || cash < amount) {
    return false;
  }
  cash -= amount;
  return true;
}

Inventory &GameState::getInventory() { return inventory; }
const Inventory &GameState::getInventory() const { return inventory; }
AutoSellSettings &GameState::getAutoSellSettings() { return autoSellSettings; }
const AutoSellSettings &GameState::getAutoSellSettings() const {
  return autoSellSettings;
}
int GameState::getPacksOpened() const { return packsOpened; }

void GameState::incrementPacksOpened() { ++packsOpened; }

PackHistoryList &GameState::getPackHistory() { return packHistory; }
const PackHistoryList &GameState::getPackHistory() const { return packHistory; }
