#include "Card.hpp"

#include "Utils.hpp"

#include <ostream>
#include <sstream>
#include <utility>
#include <vector>

using namespace std;

namespace {
struct FittedText {
  vector<string> lines;
  int fontSize = 12;
  int lineSpacing = 2;
};

vector<string> wrapTextToLines(const string &text, int maxWidth, int fontSize,
                               int maxLines) {
  vector<string> lines;
  istringstream textStream(text);
  string word;
  string currentLine;

  while (textStream >> word) {
    const string candidateLine =
        currentLine.empty() ? word : currentLine + " " + word;
    if (MeasureText(candidateLine.c_str(), fontSize) <= maxWidth) {
      currentLine = candidateLine;
      continue;
    }

    if (!currentLine.empty()) {
      lines.push_back(currentLine);
      currentLine = word;
    } else {
      lines.push_back(word);
      currentLine.clear();
    }

    if (static_cast<int>(lines.size()) == maxLines) {
      return lines;
    }
  }

  if (!currentLine.empty() && static_cast<int>(lines.size()) < maxLines) {
    lines.push_back(currentLine);
  }

  return lines;
}

string shrinkWithEllipsis(string text, int maxWidth, int fontSize) {
  const string ellipsis = "...";
  if (MeasureText(text.c_str(), fontSize) <= maxWidth) {
    return text;
  }

  while (!text.empty() &&
         MeasureText((text + ellipsis).c_str(), fontSize) > maxWidth) {
    text.pop_back();
  }

  return text.empty() ? ellipsis : text + ellipsis;
}

FittedText fitTextToArea(const string &text, int maxWidth, int maxHeight,
                         int maxLines) {
  for (int fontSize = 16; fontSize >= 9; --fontSize) {
    const int lineSpacing = fontSize <= 11 ? 1 : 2;
    vector<string> lines = wrapTextToLines(text, maxWidth, fontSize, maxLines);
    if (lines.empty()) {
      return FittedText{};
    }

    bool allLinesFit = true;
    for (const string &line : lines) {
      if (MeasureText(line.c_str(), fontSize) > maxWidth) {
        allLinesFit = false;
        break;
      }
    }

    const int textHeight = static_cast<int>(lines.size()) * fontSize +
                           static_cast<int>(lines.size() - 1) * lineSpacing;
    if (allLinesFit && textHeight <= maxHeight) {
      return FittedText{lines, fontSize, lineSpacing};
    }
  }

  FittedText fallbackText;
  fallbackText.fontSize = 9;
  fallbackText.lineSpacing = 1;
  fallbackText.lines =
      wrapTextToLines(text, maxWidth, fallbackText.fontSize, maxLines);
  if (fallbackText.lines.empty()) {
    return fallbackText;
  }
  fallbackText.lines.back() = shrinkWithEllipsis(
      fallbackText.lines.back(), maxWidth, fallbackText.fontSize);
  return fallbackText;
}

void drawCardName(const string &cardName, Rectangle bounds) {
  const int maxTextWidth = static_cast<int>(bounds.width - 24.0f);
  const int maxTextHeight = static_cast<int>(bounds.height * 0.13f);
  const FittedText fittedText =
      fitTextToArea(cardName, maxTextWidth, maxTextHeight, 2);
  if (fittedText.lines.empty()) {
    return;
  }

  const int textHeight =
      static_cast<int>(fittedText.lines.size()) * fittedText.fontSize +
      static_cast<int>(fittedText.lines.size() - 1) * fittedText.lineSpacing;
  const int areaTop = static_cast<int>(bounds.y + bounds.height * 0.49f);
  const int firstLineY = areaTop + (maxTextHeight - textHeight) / 2;
  for (int lineIndex = 0; lineIndex < static_cast<int>(fittedText.lines.size());
       ++lineIndex) {
    const int y =
        firstLineY + lineIndex * (fittedText.fontSize + fittedText.lineSpacing);
    DrawText(fittedText.lines[static_cast<size_t>(lineIndex)].c_str(),
             static_cast<int>(bounds.x + 12), y, fittedText.fontSize, RAYWHITE);
  }
}
} // namespace

int Card::totalCardsCreated = 0;

Card::Card(string id, string name, Rarity rarity, int sellValue,
           string description, string imagePath)
    : id(move(id)), name(move(name)), rarity(rarity), sellValue(sellValue),
      description(move(description)), imagePath(move(imagePath)) {
  ++totalCardsCreated;
}

Card::~Card() = default;

const string &Card::getId() const { return id; }
const string &Card::getName() const { return name; }
Rarity Card::getRarity() const { return rarity; }
int Card::getSellValue() const { return sellValue; }
const string &Card::getDescription() const { return description; }
const string &Card::getImagePath() const { return imagePath; }

void Card::draw(Rectangle bounds, bool selected) const {
  const Color rarityColor =
      rarityColors[static_cast<size_t>(static_cast<int>(rarity))];
  DrawRectangleRec(bounds, rarityColor);
  DrawRectangleLinesEx(bounds, selected ? 3.0f : 1.0f,
                       selected ? WHITE : BLACK);

  const Rectangle artBounds{bounds.x + 10.0f, bounds.y + 10.0f,
                            bounds.width - 20.0f, bounds.height * 0.42f};
  if (!imagePath.empty() && FileExists(imagePath.c_str())) {
    Texture2D cardTexture = LoadTexture(imagePath.c_str());
    if (cardTexture.id > 0) {
      DrawTexturePro(cardTexture,
                     Rectangle{0.0f, 0.0f,
                               static_cast<float>(cardTexture.width),
                               static_cast<float>(cardTexture.height)},
                     artBounds, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
      UnloadTexture(cardTexture);
    }
  } else {
    DrawRectangle(static_cast<int>(artBounds.x), static_cast<int>(artBounds.y),
                  static_cast<int>(artBounds.width),
                  static_cast<int>(artBounds.height), Color{210, 210, 210, 255});
  }

  drawCardName(name, bounds);
  DrawText(getDisplayType().c_str(), static_cast<int>(bounds.x + 12),
           static_cast<int>(bounds.y + bounds.height * 0.64f), 14,
           WHITE);
  DrawText(toString(rarity).c_str(), static_cast<int>(bounds.x + 12),
           static_cast<int>(bounds.y + bounds.height * 0.73f), 14, RAYWHITE);
  DrawText(formatCash(sellValue).c_str(), static_cast<int>(bounds.x + 12),
           static_cast<int>(bounds.y + bounds.height * 0.84f), 16,
           BLACK);
}

bool Card::operator<(const Card &other) const {
  if (sellValue == other.sellValue) {
    return static_cast<int>(rarity) < static_cast<int>(other.rarity);
  }
  return sellValue < other.sellValue;
}

bool Card::operator==(const Card &other) const { return id == other.id; }

CreatureCard::CreatureCard(string id, string name, Rarity rarity, int sellValue,
                           string description, int attackPower,
                           string imagePath)
    : Card(move(id), move(name), rarity, sellValue, move(description),
           move(imagePath)),
      attackPower(attackPower) {}

CreatureCard::~CreatureCard() = default;

string CreatureCard::getDisplayType() const {
  return "Creature ATK " + to_string(attackPower);
}

unique_ptr<Card> CreatureCard::clone() const {
  return make_unique<CreatureCard>(*this);
}

void CreatureCard::draw(Rectangle bounds, bool selected) const {
  Card::draw(bounds, selected);
}

SpellCard::SpellCard(string id, string name, Rarity rarity, int sellValue,
                     string description, int manaCost, string imagePath)
    : Card(move(id), move(name), rarity, sellValue, move(description),
           move(imagePath)),
      manaCost(manaCost) {}

SpellCard::~SpellCard() = default;

string SpellCard::getDisplayType() const {
  return "Spell Cost " + to_string(manaCost);
}

unique_ptr<Card> SpellCard::clone() const {
  return make_unique<SpellCard>(*this);
}

void SpellCard::draw(Rectangle bounds, bool selected) const {
  Card::draw(bounds, selected);
}

RelicCard::RelicCard(string id, string name, Rarity rarity, int sellValue,
                     string description, int durability, string imagePath)
    : Card(move(id), move(name), rarity, sellValue, move(description),
           move(imagePath)),
      durability(durability) {}

RelicCard::~RelicCard() = default;

string RelicCard::getDisplayType() const {
  return "Relic DUR " + to_string(durability);
}

unique_ptr<Card> RelicCard::clone() const {
  return make_unique<RelicCard>(*this);
}

void RelicCard::draw(Rectangle bounds, bool selected) const {
  Card::draw(bounds, selected);
}

ostream &operator<<(ostream &outputStream, const Card &card) {
  outputStream << card.getName() << " [" << toString(card.getRarity()) << "] "
               << formatCash(card.getSellValue());
  return outputStream;
}
