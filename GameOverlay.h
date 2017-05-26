#pragma once

#include <BadgeUI.h>
#include "defines.h"

class GameOverlay: public Overlay {
public:
  GameOverlay(game_state_t gameState): gameState(gameState) {}
  bool isDirty() {
    return dirty;
  }
  void draw(TFT_ILI9163C* tft, Theme * Theme, uint16_t offsetX, uint16_t offsetY);
  bool isValid() {
    return true;
  }
  void updateGameState(game_state_t gameState) {
    dirty = true;
    if(this->gameState == gameState) {
      return;
    }
    this->gameState = gameState;
  }
  game_state_t getGameState() {
    return this->gameState;
  }

  void setPlayerId(uint8_t id) {
    this->playerId = id;
    this->dirty = true;
  }

  uint8_t getPlayerId() {
    return this->playerId;
  }

  uint16_t getOffsetX();
  uint16_t getOffsetY();
private:
  uint8_t playerId;
  game_state_t gameState;
  bool dirty = true;
};