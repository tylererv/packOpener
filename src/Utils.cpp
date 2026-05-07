#include "Utils.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

using namespace std;

const array<string, rarityCount> rarityNames = {"Common", "Uncommon", "Rare",
                                                "Epic", "Legendary"};
const array<Color, rarityCount> rarityColors = {
    Color{164, 172, 178, 255}, Color{78, 186, 111, 255},
    Color{72, 132, 220, 255}, Color{164, 92, 219, 255},
    Color{232, 178, 67, 255}};
const array<const char *, 4> defaultTabLabels = {"Shop", "Open", "Inventory",
                                                 "Settings"};

Rarity rarityFromString(const string &rarityText) {
  string normalizedText = rarityText;
  transform(
      normalizedText.begin(), normalizedText.end(), normalizedText.begin(),
      [](unsigned char letter) { return static_cast<char>(tolower(letter)); });

  if (normalizedText == "uncommon")
    return Rarity::uncommon;
  if (normalizedText == "rare")
    return Rarity::rare;
  if (normalizedText == "epic")
    return Rarity::epic;
  if (normalizedText == "legendary")
    return Rarity::legendary;
  return Rarity::common;
}

CardType cardTypeFromString(const string &cardTypeText) {
  string normalizedText = cardTypeText;
  transform(
      normalizedText.begin(), normalizedText.end(), normalizedText.begin(),
      [](unsigned char letter) { return static_cast<char>(tolower(letter)); });

  if (normalizedText == "spell")
    return CardType::spell;
  if (normalizedText == "relic")
    return CardType::relic;
  return CardType::creature;
}

string toString(Rarity rarity) {
  const int rarityIndex = static_cast<int>(rarity);
  if (rarityIndex < 0 || rarityIndex >= rarityCount)
    return "Common";
  return rarityNames[static_cast<size_t>(rarityIndex)];
}

string toString(CardType cardType) {
  switch (cardType) {
  case CardType::spell:
    return "Spell";
  case CardType::relic:
    return "Relic";
  case CardType::creature:
  default:
    return "Creature";
  }
}

string formatCash(int amount, const string &prefix) {
  ostringstream outputStream;
  outputStream << prefix << amount;
  return outputStream.str();
}

int clampInt(int value, int minValue, int maxValue) {
  if (value < minValue)
    return minValue;
  if (value > maxValue)
    return maxValue;
  return value;
}

bool drawTextButton(Rectangle bounds, const string &label) {
  return drawTextButton(bounds, label, Color{58, 58, 58, 255}, RAYWHITE);
}

bool drawTextButton(Rectangle bounds, const string &label, Color fillColor,
                    Color textColor) {
  const Vector2 mousePosition = GetMousePosition();
  const bool isHovered = CheckCollisionPointRec(mousePosition, bounds);
  const bool isClicked = isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
  const Color buttonColor = isHovered ? Color{82, 82, 82, 255} : fillColor;

  DrawRectangleRec(bounds, buttonColor);
  DrawRectangleLinesEx(bounds, 2.0f, BLACK);

  const int fontSize = 18;
  const int textWidth = MeasureText(label.c_str(), fontSize);
  DrawText(label.c_str(),
           static_cast<int>(bounds.x + (bounds.width - textWidth) * 0.5f),
           static_cast<int>(bounds.y + (bounds.height - fontSize) * 0.5f),
           fontSize, textColor);
  return isClicked;
}

void drawWrappedText(const string &text, int x, int y, int maxWidth,
                     int fontSize, Color color) {
  string line;
  string word;
  istringstream inputStream(text);
  int currentY = y;

  while (inputStream >> word) {
    const string candidate = line.empty() ? word : line + " " + word;
    if (MeasureText(candidate.c_str(), fontSize) > maxWidth && !line.empty()) {
      DrawText(line.c_str(), x, currentY, fontSize, color);
      currentY += fontSize + 4;
      line = word;
    } else {
      line = candidate;
    }
  }

  if (!line.empty()) {
    DrawText(line.c_str(), x, currentY, fontSize, color);
  }
}

Color fadeColor(Color color, float alpha) {
  return Color{color.r, color.g, color.b,
               static_cast<unsigned char>(
                   clampInt(static_cast<int>(alpha * 255.0f), 0, 255))};
}
