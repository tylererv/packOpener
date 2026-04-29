#include "GuiController.hpp"

#include "Utils.hpp"

#include <algorithm>
#include <utility>

namespace
{
    void drawTopBar(GuiController& guiController, ScreenType activeScreen)
    {
        const GameState& gameState = guiController.getGameState();
        DrawRectangle(0, 0, 1100, 72, Color{23, 27, 33, 255});
        DrawText("packOpener", 28, 20, 28, RAYWHITE);
        DrawText(formatCash(gameState.getCash()).c_str(), 925, 22, 24, Color{116, 221, 142, 255});

        for (int tabIndex = 0; tabIndex < static_cast<int>(defaultTabLabels.size()); ++tabIndex)
        {
            const Rectangle tabBounds{220.0f + static_cast<float>(tabIndex * 132), 18.0f, 118.0f, 38.0f};
            const bool isActive = static_cast<int>(activeScreen) == tabIndex;
            if (drawTextButton(tabBounds, defaultTabLabels[static_cast<size_t>(tabIndex)], isActive ? Color{73, 88, 110, 255} : Color{37, 43, 52, 255}, RAYWHITE))
            {
                guiController.changeScreen(static_cast<ScreenType>(tabIndex));
            }
        }
    }

    void drawNavigationButtons(GuiController& guiController)
    {
        if (drawTextButton(Rectangle{224, 620, 115, 40}, "Shop")) guiController.changeScreen(ScreenType::shop);
        if (drawTextButton(Rectangle{352, 620, 115, 40}, "Open")) guiController.changeScreen(ScreenType::opening);
        if (drawTextButton(Rectangle{480, 620, 135, 40}, "Inventory")) guiController.changeScreen(ScreenType::inventory);
        if (drawTextButton(Rectangle{628, 620, 135, 40}, "Settings")) guiController.changeScreen(ScreenType::settings);
    }
}

class ShopScreen : public Screen
{
public:
    explicit ShopScreen(GuiController& guiController) : Screen(guiController) {}
    void update() override {}
    ScreenType getType() const override { return ScreenType::shop; }

    void draw() const override
    {
        drawTopBar(guiController, getType());
        DrawText("Choose a pack", 40, 100, 28, RAYWHITE);
        DrawText("Free common packs keep the run alive. Better packs cost cash and have stronger odds.", 40, 134, 18, Color{190, 198, 208, 255});

        const std::vector<Pack>& packs = guiController.getPackDatabase().getPacks();
        for (int packIndex = 0; packIndex < static_cast<int>(packs.size()); ++packIndex)
        {
            const Pack& pack = packs[static_cast<size_t>(packIndex)];
            const float x = 40.0f + static_cast<float>((packIndex % 3) * 340);
            const float y = 190.0f + static_cast<float>((packIndex / 3) * 190);
            const Rectangle packBounds{x, y, 304.0f, 180.0f};
            DrawRectangleRounded(packBounds, 0.08f, 10, Color{35, 41, 50, 255});
            DrawRectangleLinesEx(packBounds, 1.0f, Color{86, 96, 112, 255});
            DrawText(pack.getName().c_str(), static_cast<int>(x + 18), static_cast<int>(y + 16), 22, RAYWHITE);
            DrawText(("Cards: " + std::to_string(pack.getCardCount())).c_str(), static_cast<int>(x + 18), static_cast<int>(y + 50), 17, Color{220, 225, 232, 255});
            DrawText(("Price: " + formatCash(pack.getPrice())).c_str(), static_cast<int>(x + 18), static_cast<int>(y + 76), 17, Color{116, 221, 142, 255});
            drawWrappedText(pack.getOddsSummary(), static_cast<int>(x + 18), static_cast<int>(y + 103), 270, 13, Color{190, 198, 208, 255});

            const bool canAfford = guiController.getGameState().getCash() >= pack.getPrice();
            const Color buttonColor = canAfford ? Color{50, 97, 74, 255} : Color{88, 54, 54, 255};
            if (drawTextButton(Rectangle{x + 18, y + 134, 268, 34}, "Open", buttonColor, RAYWHITE))
            {
                if (guiController.getGameState().spendCash(pack.getPrice()))
                {
                    PackOpenResult result = guiController.getPackOpeningService().openPack(pack, guiController.getGameState(), guiController.getCardDatabase());
                    guiController.setLastResult(result);
                    guiController.getSaveService().saveGame(guiController.getGameState(), guiController.getCardDatabase());
                    guiController.changeScreen(ScreenType::opening);
                }
            }
        }

        const std::vector<std::string> history = guiController.getGameState().getPackHistory().getRecentEntries();
        DrawText("Recent packs", 790, 430, 22, RAYWHITE);
        for (int historyIndex = 0; historyIndex < static_cast<int>(history.size()); ++historyIndex)
        {
            DrawText(history[static_cast<size_t>(historyIndex)].c_str(), 790, 465 + historyIndex * 24, 16, Color{190, 198, 208, 255});
        }
        drawNavigationButtons(guiController);
    }
};

class PackOpenScreen : public Screen
{
public:
    explicit PackOpenScreen(GuiController& guiController) : Screen(guiController) {}
    void update() override {}
    ScreenType getType() const override { return ScreenType::opening; }

    void draw() const override
    {
        drawTopBar(guiController, getType());
        const PackOpenResult& result = guiController.getLastResult();
        DrawText("Latest opening", 40, 100, 28, RAYWHITE);

        if (result.cardIds.empty())
        {
            DrawText("Open a pack from the shop to reveal cards here.", 40, 142, 20, Color{190, 198, 208, 255});
        }
        else
        {
            DrawText(result.packName.c_str(), 40, 142, 22, Color{220, 225, 232, 255});
            if (result.autoSoldCount > 0)
            {
                const std::string autoSellText = "Auto-sold " + std::to_string(result.autoSoldCount) + " cards for " + formatCash(result.autoSoldValue);
                DrawText(autoSellText.c_str(), 40, 174, 18, Color{116, 221, 142, 255});
            }

            for (int resultIndex = 0; resultIndex < static_cast<int>(result.cardIds.size()); ++resultIndex)
            {
                const Card* card = guiController.getCardDatabase().findCard(result.cardIds[static_cast<size_t>(resultIndex)]);
                if (card == nullptr) continue;

                const float x = 40.0f + static_cast<float>((resultIndex % 6) * 170);
                const float y = 220.0f + static_cast<float>((resultIndex / 6) * 180);
                card->draw(Rectangle{x, y, 145.0f, 160.0f});
            }
        }

        drawNavigationButtons(guiController);
    }
};

class InventoryScreen : public Screen
{
public:
    explicit InventoryScreen(GuiController& guiController) : Screen(guiController) {}
    void update() override {}
    ScreenType getType() const override { return ScreenType::inventory; }

    void draw() const override
    {
        drawTopBar(guiController, getType());
        DrawText("Inventory", 40, 100, 28, RAYWHITE);

        const int totalValue = guiController.getGameState().getInventory().calculateTotalValue(guiController.getCardDatabase());
        DrawText(("Total sell value: " + formatCash(totalValue)).c_str(), 40, 138, 18, Color{190, 198, 208, 255});

        if (drawTextButton(Rectangle{760, 104, 170, 40}, "Sell duplicates", Color{94, 79, 45, 255}, RAYWHITE))
        {
            const int gainedCash = guiController.getGameState().getInventory().sellDuplicates(guiController.getCardDatabase());
            guiController.getGameState().addCash(gainedCash);
            guiController.getSaveService().saveGame(guiController.getGameState(), guiController.getCardDatabase());
        }

        const std::vector<InventoryEntry> entries = guiController.getGameState().getInventory().getEntries();
        int visibleIndex = 0;
        for (const InventoryEntry& entry : entries)
        {
            const Card* card = guiController.getCardDatabase().findCard(entry.cardId);
            if (card == nullptr) continue;

            const float y = 184.0f + static_cast<float>(visibleIndex * 48);
            if (y > 570.0f) break;

            DrawRectangleRounded(Rectangle{40, y, 900, 38}, 0.08f, 8, Color{35, 41, 50, 255});
            DrawText(card->getName().c_str(), 58, static_cast<int>(y + 10), 16, RAYWHITE);
            DrawText(card->getDisplayType().c_str(), 300, static_cast<int>(y + 10), 16, Color{190, 198, 208, 255});
            DrawText(("x" + std::to_string(entry.count)).c_str(), 525, static_cast<int>(y + 10), 16, RAYWHITE);
            DrawText(formatCash(card->getSellValue()).c_str(), 600, static_cast<int>(y + 10), 16, Color{116, 221, 142, 255});

            if (drawTextButton(Rectangle{802, y + 4, 112, 30}, "Sell one", Color{75, 49, 49, 255}, RAYWHITE))
            {
                const int gainedCash = guiController.getGameState().getInventory().sellCard(entry.cardId, guiController.getCardDatabase());
                guiController.getGameState().addCash(gainedCash);
                guiController.getSaveService().saveGame(guiController.getGameState(), guiController.getCardDatabase());
            }
            ++visibleIndex;
        }

        if (entries.empty())
        {
            DrawText("No cards yet. Open packs to build your collection.", 40, 184, 20, Color{190, 198, 208, 255});
        }

        drawNavigationButtons(guiController);
    }
};

class SettingsScreen : public Screen
{
public:
    explicit SettingsScreen(GuiController& guiController) : Screen(guiController) {}
    void update() override {}
    ScreenType getType() const override { return ScreenType::settings; }

    void draw() const override
    {
        drawTopBar(guiController, getType());
        AutoSellSettings& settings = guiController.getGameState().getAutoSellSettings();

        DrawText("Auto-sell settings", 40, 100, 28, RAYWHITE);
        DrawText("Auto-sell applies immediately after a pack is opened.", 40, 138, 18, Color{190, 198, 208, 255});

        if (drawTextButton(Rectangle{40, 190, 240, 42}, settings.enabled ? "Auto-sell: on" : "Auto-sell: off"))
        {
            settings.enabled = !settings.enabled;
            if (settings.enabled && !settings.sellByRarity && !settings.sellByValue)
            {
                settings.sellByRarity = true;
                settings.maxRarity = Rarity::common;
            }
            guiController.getSaveService().saveGame(guiController.getGameState(), guiController.getCardDatabase());
        }

        if (drawTextButton(Rectangle{40, 250, 240, 42}, settings.sellByRarity ? "Rarity rule: on" : "Rarity rule: off"))
        {
            settings.sellByRarity = !settings.sellByRarity;
            if (settings.sellByRarity)
            {
                settings.enabled = true;
            }
            guiController.getSaveService().saveGame(guiController.getGameState(), guiController.getCardDatabase());
        }

        if (drawTextButton(Rectangle{310, 250, 180, 42}, "Cycle rarity"))
        {
            const int nextRarity = (static_cast<int>(settings.maxRarity) + 1) % rarityCount;
            settings.maxRarity = static_cast<Rarity>(nextRarity);
            guiController.getSaveService().saveGame(guiController.getGameState(), guiController.getCardDatabase());
        }
        DrawText(("Sell rarity <= " + toString(settings.maxRarity)).c_str(), 520, 262, 18, RAYWHITE);

        if (drawTextButton(Rectangle{40, 320, 240, 42}, settings.sellByValue ? "Value rule: on" : "Value rule: off"))
        {
            settings.sellByValue = !settings.sellByValue;
            if (settings.sellByValue)
            {
                settings.enabled = true;
            }
            guiController.getSaveService().saveGame(guiController.getGameState(), guiController.getCardDatabase());
        }

        bool valueSettingsChanged = false;
        if (drawTextButton(Rectangle{310, 320, 72, 38}, "- Min"))
        {
            settings.minValue = clampInt(settings.minValue - 5, 0, 999);
            valueSettingsChanged = true;
        }
        if (drawTextButton(Rectangle{390, 320, 72, 38}, "+ Min"))
        {
            settings.minValue = clampInt(settings.minValue + 5, 0, 999);
            valueSettingsChanged = true;
        }
        if (drawTextButton(Rectangle{500, 320, 72, 38}, "- Max"))
        {
            settings.maxValue = clampInt(settings.maxValue - 5, settings.minValue, 999);
            valueSettingsChanged = true;
        }
        if (drawTextButton(Rectangle{580, 320, 72, 38}, "+ Max"))
        {
            settings.maxValue = clampInt(settings.maxValue + 5, settings.minValue, 999);
            valueSettingsChanged = true;
        }
        if (valueSettingsChanged)
        {
            guiController.getSaveService().saveGame(guiController.getGameState(), guiController.getCardDatabase());
        }
        DrawText(("Sell value " + formatCash(settings.minValue) + " to " + formatCash(settings.maxValue)).c_str(), 680, 330, 18, RAYWHITE);

        DrawText(("Cards created: " + std::to_string(Card::totalCardsCreated)).c_str(), 40, 420, 18, Color{190, 198, 208, 255});
        DrawText(("Packs opened this run: " + std::to_string(PackOpeningService::totalPacksOpened)).c_str(), 40, 450, 18, Color{190, 198, 208, 255});
        DrawText(("Saved packs opened: " + std::to_string(guiController.getGameState().getPacksOpened())).c_str(), 40, 480, 18, Color{190, 198, 208, 255});

        drawNavigationButtons(guiController);
    }
};

Screen::Screen(GuiController& guiController)
    : guiController(guiController)
{
}

Screen::~Screen() = default;

GuiController::GuiController(CardDatabase& cardDatabase, PackDatabase& packDatabase, GameState& gameState, SaveService& saveService, std::string savePath)
    : cardDatabase(cardDatabase),
      packDatabase(packDatabase),
      gameState(gameState),
      saveService(saveService),
      packOpeningService(),
      activeScreen(nullptr),
      lastResult(),
      savePath(std::move(savePath)),
      pendingScreenType(ScreenType::shop),
      hasPendingScreenChange(false)
{
    activeScreen = createScreen(ScreenType::shop);
}

GuiController::~GuiController() = default;

void GuiController::update()
{
    applyPendingScreenChange();
    if (activeScreen)
    {
        activeScreen->update();
    }
}

void GuiController::draw()
{
    if (activeScreen)
    {
        activeScreen->draw();
    }
}

void GuiController::changeScreen(ScreenType screenType)
{
    pendingScreenType = screenType;
    hasPendingScreenChange = true;
}

void GuiController::setLastResult(const PackOpenResult& result)
{
    lastResult = result;
}

const PackOpenResult& GuiController::getLastResult() const { return lastResult; }
CardDatabase& GuiController::getCardDatabase() { return cardDatabase; }
PackDatabase& GuiController::getPackDatabase() { return packDatabase; }
GameState& GuiController::getGameState() { return gameState; }
PackOpeningService& GuiController::getPackOpeningService() { return packOpeningService; }
SaveService& GuiController::getSaveService() { return saveService; }
const std::string& GuiController::getSavePath() const { return savePath; }

void GuiController::applyPendingScreenChange()
{
    if (!hasPendingScreenChange)
    {
        return;
    }

    activeScreen = createScreen(pendingScreenType);
    hasPendingScreenChange = false;
}

std::unique_ptr<Screen> GuiController::createScreen(ScreenType screenType)
{
    switch (screenType)
    {
        case ScreenType::opening: return std::make_unique<PackOpenScreen>(*this);
        case ScreenType::inventory: return std::make_unique<InventoryScreen>(*this);
        case ScreenType::settings: return std::make_unique<SettingsScreen>(*this);
        case ScreenType::shop:
        default: return std::make_unique<ShopScreen>(*this);
    }
}
