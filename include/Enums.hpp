#pragma once

#include "raylib.h"

#include <array>
#include <string>

enum class Rarity { common = 0, uncommon, rare, epic, legendary };

enum class CardType { creature = 0, spell, relic };

enum class ScreenType { shop = 0, opening, inventory, settings };

constexpr int rarityCount = 5;

extern const std::array<std::string, rarityCount> rarityNames;
extern const std::array<Color, rarityCount> rarityColors;
extern const std::array<const char *, 4> defaultTabLabels;

Rarity rarityFromString(const std::string &rarityText);
CardType cardTypeFromString(const std::string &cardTypeText);
std::string toString(Rarity rarity);
std::string toString(CardType cardType);
