#include "Utils.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

const std::array<std::string, rarityCount> rarityNames = {"Common", "Uncommon", "Rare", "Epic", "Legendary"};
const std::array<Color, rarityCount> rarityColors = {
    Color{164, 172, 178, 255},
    Color{78, 186, 111, 255},
    Color{72, 132, 220, 255},
    Color{164, 92, 219, 255},
    Color{232, 178, 67, 255}
};
const std::array<const char*, 4> defaultTabLabels = {"Shop", "Open", "Inventory", "Settings"};

Rarity rarityFromString(const std::string& rarityText)
{
    std::string normalizedText = rarityText;
    std::transform(normalizedText.begin(), normalizedText.end(), normalizedText.begin(), [](unsigned char letter) {
        return static_cast<char>(std::tolower(letter));
    });

    if (normalizedText == "uncommon") return Rarity::uncommon;
    if (normalizedText == "rare") return Rarity::rare;
    if (normalizedText == "epic") return Rarity::epic;
    if (normalizedText == "legendary") return Rarity::legendary;
    return Rarity::common;
}

CardType cardTypeFromString(const std::string& cardTypeText)
{
    std::string normalizedText = cardTypeText;
    std::transform(normalizedText.begin(), normalizedText.end(), normalizedText.begin(), [](unsigned char letter) {
        return static_cast<char>(std::tolower(letter));
    });

    if (normalizedText == "spell") return CardType::spell;
    if (normalizedText == "relic") return CardType::relic;
    return CardType::creature;
}

std::string toString(Rarity rarity)
{
    const int rarityIndex = static_cast<int>(rarity);
    if (rarityIndex < 0 || rarityIndex >= rarityCount) return "Common";
    return rarityNames[static_cast<size_t>(rarityIndex)];
}

std::string toString(CardType cardType)
{
    switch (cardType)
    {
        case CardType::spell: return "Spell";
        case CardType::relic: return "Relic";
        case CardType::creature:
        default: return "Creature";
    }
}

std::string formatCash(int amount, const std::string& prefix)
{
    std::ostringstream outputStream;
    outputStream << prefix << amount;
    return outputStream.str();
}

int clampInt(int value, int minValue, int maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

bool drawTextButton(Rectangle bounds, const std::string& label)
{
    return drawTextButton(bounds, label, Color{42, 48, 58, 255}, RAYWHITE);
}

bool drawTextButton(Rectangle bounds, const std::string& label, Color fillColor, Color textColor)
{
    const Vector2 mousePosition = GetMousePosition();
    const bool isHovered = CheckCollisionPointRec(mousePosition, bounds);
    const bool isClicked = isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    const Color buttonColor = isHovered ? Color{static_cast<unsigned char>(clampInt(fillColor.r + 25, 0, 255)), static_cast<unsigned char>(clampInt(fillColor.g + 25, 0, 255)), static_cast<unsigned char>(clampInt(fillColor.b + 25, 0, 255)), fillColor.a} : fillColor;

    DrawRectangleRounded(bounds, 0.12f, 8, buttonColor);
    DrawRectangleLinesEx(bounds, 1.0f, Color{95, 105, 118, 255});

    const int fontSize = 18;
    const int textWidth = MeasureText(label.c_str(), fontSize);
    DrawText(label.c_str(), static_cast<int>(bounds.x + (bounds.width - textWidth) * 0.5f), static_cast<int>(bounds.y + (bounds.height - fontSize) * 0.5f), fontSize, textColor);
    return isClicked;
}

void drawWrappedText(const std::string& text, int x, int y, int maxWidth, int fontSize, Color color)
{
    std::string line;
    std::string word;
    std::istringstream inputStream(text);
    int currentY = y;

    while (inputStream >> word)
    {
        const std::string candidate = line.empty() ? word : line + " " + word;
        if (MeasureText(candidate.c_str(), fontSize) > maxWidth && !line.empty())
        {
            DrawText(line.c_str(), x, currentY, fontSize, color);
            currentY += fontSize + 4;
            line = word;
        }
        else
        {
            line = candidate;
        }
    }

    if (!line.empty())
    {
        DrawText(line.c_str(), x, currentY, fontSize, color);
    }
}

Color fadeColor(Color color, float alpha)
{
    return Color{color.r, color.g, color.b, static_cast<unsigned char>(clampInt(static_cast<int>(alpha * 255.0f), 0, 255))};
}
