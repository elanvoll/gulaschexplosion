#pragma once

#include <GPNBadge.hpp>
#include <BadgeUI.h>
#include "GameOverlay.h"
#include "Packets.h"


class GameUI: public UIElement {
public:
  GameUI(GameOverlay* statusOverlay, Badge* badge);
  ~GameUI();
  void draw(TFT_ILI9163C* tft, Theme * theme, uint16_t offsetX, uint16_t offsetY);

  bool isDirty() {
    return dirty;
  }

  void dispatchInput(JoystickState state) {
    Serial.println("dispatchInput");
    if (statusOverlay->getGameState() == GAME_STATE_HOST_AWAIT_START) {
      if(state == JoystickState::BTN_ENTER) {
        onStart();
      }
    } else if (statusOverlay->getGameState() == GAME_STATE_FINISH) {
      if (state == JoystickState::BTN_ENTER) {
        if (onContinue) onContinue();
      }
    } else if (statusOverlay->getGameState() == GAME_STATE_RUNNING){
      switch (state) {
        case JoystickState::BTN_ENTER:
          if (onEnter) onEnter();
          break;
        case JoystickState::BTN_LEFT:
          if (onLeft) onLeft();
          break;
        case JoystickState::BTN_RIGHT:
          if (onRight) onRight();
          break;
        case JoystickState::BTN_UP:
          if (onUp) onUp();
          break;
        case JoystickState::BTN_DOWN:
          if (onDown) onDown();
          break;
      }
    }
  }

  void updateGameState(game_state_t t) {
    statusOverlay->updateGameState(t);
    badge->setVibrator(true);
    delay(42);
    badge->setVibrator(false);
    dirty = true;
  }

  void setOnGameStart(std::function<void()> onStart) {
    this->onStart = onStart;
  }

  void setOnContinue(std::function<void()> onContinue) {
    this->onContinue = onContinue;
  }

  void setOnPushEnter(std::function<void()> onEnter) {
    this->onEnter = onEnter;
  }

  void setOnPushLeft(std::function<void()> onLeft) {
    this->onLeft = onLeft;
  }

  void setOnPushRight(std::function<void()> onRight) {
    this->onRight = onRight;
  }

  void setOnPushUp(std::function<void()> onUp) {
    this->onUp = onUp;
  }

  void setOnPushDown(std::function<void()> onDown) {
    this->onDown = onDown;
  }

  void serverNewUser(uint8_t id) {
    playerCount++;
    dirty = true;
    if (playerCount == PLAYERS) {
      updateGameState(GAME_STATE_HOST_AWAIT_START);
    }
  }

  // all Handlers
  void handleJoin(ServerJoinAckPacket* packet);
  void handleGameStart(ServerGameStartPacket* packet);
  void handleLogClientAction(ServerClientActionLogPacket* packet);
  void handleGameSuccess(ServerGameSuccess* packet);
  void handleGameOver(SeverGameOver* packet);

  void doTime();
  void vib(uint16_t);
  void resetLEDs();

private:
  String lastPacketMessage;
  String lastLogMessage;
  unsigned long lastTime = 0;
  uint32_t remainingTime = 0;
  uint16_t remainingVib = 0;

  GameOverlay* statusOverlay;
  Badge* badge;
  uint8_t player;
  uint8_t playerCount = 0;
  bool dirty = true;
  std::function<void()> onStart;
  std::function<void()> onContinue;
  std::function<void()> onEnter;
  std::function<void()> onLeft;
  std::function<void()> onRight;
  std::function<void()> onUp;
  std::function<void()> onDown;
};
