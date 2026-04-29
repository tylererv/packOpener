#pragma once

#include "Enums.hpp"

#include <iosfwd>
#include <memory>
#include <string>

class Card
{
public:
    static int totalCardsCreated;

    Card(std::string id, std::string name, Rarity rarity, int sellValue, std::string description, std::string imagePath = "");
    Card(const Card& other) = default;
    virtual ~Card();

    const std::string& getId() const;
    const std::string& getName() const;
    Rarity getRarity() const;
    int getSellValue() const;
    const std::string& getDescription() const;
    const std::string& getImagePath() const;

    virtual std::string getDisplayType() const = 0;
    virtual std::unique_ptr<Card> clone() const = 0;
    virtual void draw(Rectangle bounds, bool selected = false) const;

    bool operator<(const Card& other) const;
    bool operator==(const Card& other) const;

protected:
    const std::string id;
    const std::string name;
    const Rarity rarity;
    const int sellValue;
    const std::string description;
    const std::string imagePath;
};

class CreatureCard : public Card
{
public:
    CreatureCard(std::string id, std::string name, Rarity rarity, int sellValue, std::string description, int attackPower, std::string imagePath = "");
    CreatureCard(const CreatureCard& other) = default;
    ~CreatureCard() override;

    std::string getDisplayType() const override;
    std::unique_ptr<Card> clone() const override;
    void draw(Rectangle bounds, bool selected = false) const override;

private:
    int attackPower;
};

class SpellCard : public Card
{
public:
    SpellCard(std::string id, std::string name, Rarity rarity, int sellValue, std::string description, int manaCost, std::string imagePath = "");
    SpellCard(const SpellCard& other) = default;
    ~SpellCard() override;

    std::string getDisplayType() const override;
    std::unique_ptr<Card> clone() const override;
    void draw(Rectangle bounds, bool selected = false) const override;

private:
    int manaCost;
};

class RelicCard : public Card
{
public:
    RelicCard(std::string id, std::string name, Rarity rarity, int sellValue, std::string description, int durability, std::string imagePath = "");
    RelicCard(const RelicCard& other) = default;
    ~RelicCard() override;

    std::string getDisplayType() const override;
    std::unique_ptr<Card> clone() const override;
    void draw(Rectangle bounds, bool selected = false) const override;

private:
    int durability;
};

std::ostream& operator<<(std::ostream& outputStream, const Card& card);
