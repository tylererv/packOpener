#include "Card.hpp"

#include "Utils.hpp"

#include <ostream>
#include <utility>

int Card::totalCardsCreated = 0;

Card::Card(std::string id, std::string name, Rarity rarity, int sellValue, std::string description, std::string imagePath)
    : id(std::move(id)), name(std::move(name)), rarity(rarity), sellValue(sellValue), description(std::move(description)), imagePath(std::move(imagePath))
{
    ++totalCardsCreated;
}

Card::~Card() = default;

const std::string& Card::getId() const { return id; }
const std::string& Card::getName() const { return name; }
Rarity Card::getRarity() const { return rarity; }
int Card::getSellValue() const { return sellValue; }
const std::string& Card::getDescription() const { return description; }
const std::string& Card::getImagePath() const { return imagePath; }

void Card::draw(Rectangle bounds, bool selected) const
{
    const Color rarityColor = rarityColors[static_cast<size_t>(static_cast<int>(rarity))];
    DrawRectangleRounded(bounds, 0.08f, 10, fadeColor(rarityColor, 0.92f));
    DrawRectangleLinesEx(bounds, selected ? 3.0f : 1.0f, selected ? WHITE : Color{35, 39, 45, 255});

    const Rectangle artBounds{bounds.x + 10.0f, bounds.y + 10.0f, bounds.width - 20.0f, bounds.height * 0.42f};
    if (!imagePath.empty() && FileExists(imagePath.c_str()))
    {
        Texture2D cardTexture = LoadTexture(imagePath.c_str());
        if (cardTexture.id > 0)
        {
            DrawTexturePro(cardTexture, Rectangle{0.0f, 0.0f, static_cast<float>(cardTexture.width), static_cast<float>(cardTexture.height)}, artBounds, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
            UnloadTexture(cardTexture);
        }
    }
    else
    {
        DrawRectangle(static_cast<int>(artBounds.x), static_cast<int>(artBounds.y), static_cast<int>(artBounds.width), static_cast<int>(artBounds.height), fadeColor(WHITE, 0.18f));
    }

    DrawText(name.c_str(), static_cast<int>(bounds.x + 12), static_cast<int>(bounds.y + bounds.height * 0.50f), 16, RAYWHITE);
    DrawText(getDisplayType().c_str(), static_cast<int>(bounds.x + 12), static_cast<int>(bounds.y + bounds.height * 0.62f), 14, Color{235, 238, 242, 255});
    DrawText(toString(rarity).c_str(), static_cast<int>(bounds.x + 12), static_cast<int>(bounds.y + bounds.height * 0.73f), 14, RAYWHITE);
    DrawText(formatCash(sellValue).c_str(), static_cast<int>(bounds.x + 12), static_cast<int>(bounds.y + bounds.height * 0.84f), 16, Color{30, 34, 38, 255});
}

bool Card::operator<(const Card& other) const
{
    if (sellValue == other.sellValue)
    {
        return static_cast<int>(rarity) < static_cast<int>(other.rarity);
    }
    return sellValue < other.sellValue;
}

bool Card::operator==(const Card& other) const
{
    return id == other.id;
}

CreatureCard::CreatureCard(std::string id, std::string name, Rarity rarity, int sellValue, std::string description, int attackPower, std::string imagePath)
    : Card(std::move(id), std::move(name), rarity, sellValue, std::move(description), std::move(imagePath)), attackPower(attackPower)
{
}

CreatureCard::~CreatureCard() = default;

std::string CreatureCard::getDisplayType() const
{
    return "Creature ATK " + std::to_string(attackPower);
}

std::unique_ptr<Card> CreatureCard::clone() const
{
    return std::make_unique<CreatureCard>(*this);
}

void CreatureCard::draw(Rectangle bounds, bool selected) const
{
    Card::draw(bounds, selected);
}

SpellCard::SpellCard(std::string id, std::string name, Rarity rarity, int sellValue, std::string description, int manaCost, std::string imagePath)
    : Card(std::move(id), std::move(name), rarity, sellValue, std::move(description), std::move(imagePath)), manaCost(manaCost)
{
}

SpellCard::~SpellCard() = default;

std::string SpellCard::getDisplayType() const
{
    return "Spell Cost " + std::to_string(manaCost);
}

std::unique_ptr<Card> SpellCard::clone() const
{
    return std::make_unique<SpellCard>(*this);
}

void SpellCard::draw(Rectangle bounds, bool selected) const
{
    Card::draw(bounds, selected);
}

RelicCard::RelicCard(std::string id, std::string name, Rarity rarity, int sellValue, std::string description, int durability, std::string imagePath)
    : Card(std::move(id), std::move(name), rarity, sellValue, std::move(description), std::move(imagePath)), durability(durability)
{
}

RelicCard::~RelicCard() = default;

std::string RelicCard::getDisplayType() const
{
    return "Relic DUR " + std::to_string(durability);
}

std::unique_ptr<Card> RelicCard::clone() const
{
    return std::make_unique<RelicCard>(*this);
}

void RelicCard::draw(Rectangle bounds, bool selected) const
{
    Card::draw(bounds, selected);
}

std::ostream& operator<<(std::ostream& outputStream, const Card& card)
{
    outputStream << card.getName() << " [" << toString(card.getRarity()) << "] " << formatCash(card.getSellValue());
    return outputStream;
}
