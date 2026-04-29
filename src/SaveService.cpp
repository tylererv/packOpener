#include "SaveService.hpp"

#include "Utils.hpp"

#include "raylib.h"

#include <cstring>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

namespace {
// raylib 5.x removed SaveStorageValue/LoadStorageValue.
// These helpers replicate the original behaviour: a flat binary file of
// ints, indexed by an unsigned int slot key.

constexpr const char *storageFilePath = "saves/storage.data";

// Returns the current storage buffer, resized to hold at least (position+1)
// ints.
vector<int> readStorageFile() {
  int dataSize = 0;
  unsigned char *raw = LoadFileData(storageFilePath, &dataSize);
  vector<int> buf;
  if (raw && dataSize > 0) {
    const size_t count = static_cast<size_t>(dataSize) / sizeof(int);
    buf.resize(count);
    memcpy(buf.data(), raw, count * sizeof(int));
    UnloadFileData(raw);
  }
  return buf;
}

bool SaveStorageValue(unsigned int position, int value) {
  vector<int> buf = readStorageFile();
  if (position >= buf.size())
    buf.resize(position + 1, 0);
  buf[position] = value;
  return SaveFileData(storageFilePath, buf.data(),
                      static_cast<int>(buf.size() * sizeof(int)));
}

int LoadStorageValue(unsigned int position) {
  const vector<int> buf = readStorageFile();
  if (position < buf.size())
    return buf[position];
  return 0;
}
} // namespace

namespace {
constexpr unsigned int cashStorageKey = 0;
constexpr unsigned int packsOpenedStorageKey = 1;
constexpr unsigned int autoSellEnabledStorageKey = 2;
constexpr unsigned int autoSellRarityEnabledStorageKey = 3;
constexpr unsigned int autoSellValueEnabledStorageKey = 4;
constexpr unsigned int autoSellMaxRarityStorageKey = 5;
constexpr unsigned int autoSellMinValueStorageKey = 6;
constexpr unsigned int autoSellMaxValueStorageKey = 7;
constexpr unsigned int inventoryStorageStartKey = 100;
} // namespace

bool SaveService::loadGame(GameState &gameState,
                           const CardDatabase &cardDatabase) {
  GameState loadedState;
  loadedState.addCash(LoadStorageValue(cashStorageKey));

  const int packsOpened = LoadStorageValue(packsOpenedStorageKey);
  for (int packIndex = 0; packIndex < packsOpened; ++packIndex) {
    loadedState.incrementPacksOpened();
  }

  AutoSellSettings &settings = loadedState.getAutoSellSettings();
  settings.enabled = LoadStorageValue(autoSellEnabledStorageKey) == 1;
  settings.sellByRarity =
      LoadStorageValue(autoSellRarityEnabledStorageKey) == 1;
  settings.sellByValue = LoadStorageValue(autoSellValueEnabledStorageKey) == 1;
  settings.maxRarity = static_cast<Rarity>(clampInt(
      LoadStorageValue(autoSellMaxRarityStorageKey), 0, rarityCount - 1));
  settings.minValue =
      clampInt(LoadStorageValue(autoSellMinValueStorageKey), 0, 999);
  settings.maxValue = clampInt(LoadStorageValue(autoSellMaxValueStorageKey),
                               settings.minValue, 999);

  unordered_map<string, int> cardCounts;
  const auto &cards = cardDatabase.getCards();
  for (int cardIndex = 0; cardIndex < static_cast<int>(cards.size());
       ++cardIndex) {
    const int count = LoadStorageValue(inventoryStorageStartKey +
                                       static_cast<unsigned int>(cardIndex));
    if (count > 0) {
      cardCounts[cards[static_cast<size_t>(cardIndex)]->getId()] = count;
    }
  }
  loadedState.getInventory().setCardCounts(cardCounts);

  gameState = loadedState;
  return true;
}

bool SaveService::saveGame(const GameState &gameState,
                           const CardDatabase &cardDatabase) const {
  bool savedSuccessfully = true;
  savedSuccessfully = SaveStorageValue(cashStorageKey, gameState.getCash()) &&
                      savedSuccessfully;
  savedSuccessfully =
      SaveStorageValue(packsOpenedStorageKey, gameState.getPacksOpened()) &&
      savedSuccessfully;

  const AutoSellSettings &settings = gameState.getAutoSellSettings();
  savedSuccessfully =
      SaveStorageValue(autoSellEnabledStorageKey, settings.enabled ? 1 : 0) &&
      savedSuccessfully;
  savedSuccessfully = SaveStorageValue(autoSellRarityEnabledStorageKey,
                                       settings.sellByRarity ? 1 : 0) &&
                      savedSuccessfully;
  savedSuccessfully = SaveStorageValue(autoSellValueEnabledStorageKey,
                                       settings.sellByValue ? 1 : 0) &&
                      savedSuccessfully;
  savedSuccessfully = SaveStorageValue(autoSellMaxRarityStorageKey,
                                       static_cast<int>(settings.maxRarity)) &&
                      savedSuccessfully;
  savedSuccessfully =
      SaveStorageValue(autoSellMinValueStorageKey, settings.minValue) &&
      savedSuccessfully;
  savedSuccessfully =
      SaveStorageValue(autoSellMaxValueStorageKey, settings.maxValue) &&
      savedSuccessfully;

  const auto &cards = cardDatabase.getCards();
  for (int cardIndex = 0; cardIndex < static_cast<int>(cards.size());
       ++cardIndex) {
    const string &cardId = cards[static_cast<size_t>(cardIndex)]->getId();
    const int count = gameState.getInventory().getCount(cardId);
    savedSuccessfully =
        SaveStorageValue(inventoryStorageStartKey +
                             static_cast<unsigned int>(cardIndex),
                         count) &&
        savedSuccessfully;
  }

  if (!savedSuccessfully) {
    cerr << "One or more raylib storage values failed to save.\n";
  }
  return savedSuccessfully;
}
