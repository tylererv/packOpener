#include "CardDatabase.hpp"
#include "GameState.hpp"
#include "GuiController.hpp"
#include "PackDatabase.hpp"
#include "SaveService.hpp"

#include "raylib-cpp.hpp"

#include <iostream>
#include <string>

using namespace std;

int main() {
  const int screenWidth = 1100;
  const int screenHeight = 700;
  const string cardDataPath = "data/cards.txt";
  const string packDataPath = "data/packs.txt";

  CardDatabase cardDatabase;
  PackDatabase packDatabase;
  GameState gameState;
  SaveService saveService;

  if (!cardDatabase.loadFromFile(cardDataPath) ||
      !packDatabase.loadFromFile(packDataPath)) {
    cerr << "Failed to load required game data.\n";
    return 1;
  }

  saveService.loadGame(gameState, cardDatabase);

  raylib::Window window(screenWidth, screenHeight, "packOpener");
  SetTargetFPS(60);

  GuiController guiController(cardDatabase, packDatabase, gameState,
                              saveService, "raylib-storage");

  while (!window.ShouldClose()) {
    guiController.update();

    BeginDrawing();
    ClearBackground(Color{18, 21, 26, 255});
    guiController.draw();
    EndDrawing();
  }

  // Save game before exiting
  saveService.saveGame(gameState, cardDatabase);
  return 0;
}
