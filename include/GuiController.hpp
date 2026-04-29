#pragma once

#include "CardDatabase.hpp"
#include "GameState.hpp"
#include "PackDatabase.hpp"
#include "PackOpeningService.hpp"
#include "SaveService.hpp"

#include <memory>
#include <string>
#include <vector>

class GuiController;

class Screen {
public:
  explicit Screen(GuiController &guiController);
  virtual ~Screen();

  virtual void update() = 0;
  virtual void draw() const = 0;
  virtual ScreenType getType() const = 0;

protected:
  GuiController &guiController;
};

class GuiController {
public:
  GuiController(CardDatabase &cardDatabase, PackDatabase &packDatabase,
                GameState &gameState, SaveService &saveService,
                std::string savePath);
  ~GuiController();

  void update();
  void draw();
  void changeScreen(ScreenType screenType);
  void setLastResult(const PackOpenResult &result);
  const PackOpenResult &getLastResult() const;
  float getRevealDelay() const;
  void adjustRevealDelay(float deltaSeconds);

  CardDatabase &getCardDatabase();
  PackDatabase &getPackDatabase();
  GameState &getGameState();
  PackOpeningService &getPackOpeningService();
  SaveService &getSaveService();
  const std::string &getSavePath() const;

private:
  void applyPendingScreenChange();
  std::unique_ptr<Screen> createScreen(ScreenType screenType);

  CardDatabase &cardDatabase;
  PackDatabase &packDatabase;
  GameState &gameState;
  SaveService &saveService;
  PackOpeningService packOpeningService;
  std::unique_ptr<Screen> activeScreen;
  PackOpenResult lastResult;
  std::string savePath;
  ScreenType pendingScreenType;
  bool hasPendingScreenChange;
  float revealDelay;
};
