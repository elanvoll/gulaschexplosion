#pragma once

#include <GPNBadge.hpp>
#include <BadgeUI.h>
#include "Packets.h"

class GameUI: public UIElement {
public:
  GameUI();
  ~GameUI();
  void draw(TFT_ILI9163C* tft, Theme * theme, uint16_t offsetX, uint16_t offsetY);

  bool isDirty() {
    return dirty;
  }

  void dispatchInput(JoystickState state) {
    if (false) {
      if(state == JoystickState::BTN_ENTER) {
        onStart();
      }
    } else {
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

  void setOnGameStart(std::function<void()> onStart) {
    this->onStart = onStart;
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

  // all Handlers
  void handleJoin(ServerJoinAckPacket* packet);


private:
  uint8 player;
  bool dirty = false;
  std::function<void()> onStart;
  std::function<void()> onEnter;
  std::function<void()> onLeft;
  std::function<void()> onRight;
  std::function<void()> onUp;
  std::function<void()> onDown;
};