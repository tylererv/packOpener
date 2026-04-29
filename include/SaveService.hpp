#pragma once

#include "CardDatabase.hpp"
#include "GameState.hpp"

#include <string>

class SaveService
{
public:
    bool loadGame(GameState& gameState, const CardDatabase& cardDatabase);
    bool saveGame(const GameState& gameState, const CardDatabase& cardDatabase) const;
};
