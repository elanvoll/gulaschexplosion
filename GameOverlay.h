#include <BadgeUI.h>


class GameOverlay: public Overlay {
public:
  GameOverlay(uint16_t gameState): gameState(gameState) {}
  bool isDirty() {
    return dirty;
  }
  void draw(TFT_ILI9163C* tft, Theme * Theme, uint16_t offsetX, uint16_t offsetY);
  bool isValid() {
    return true;
  }
  void updateGameState(uint16_t gameState) {
    if(this->gameState == gameState) {
      return;
    }
    this->gameState = gameState;
    this->dirty = true;
  }

  uint16_t getOffsetX();
  uint16_t getOffsetY();
private:
  uint16_t gameState;
  bool dirty = true;
};