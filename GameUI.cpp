#include "GameUI.h"
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans7pt7b.h>
#include <Fonts/Org_01.h>
#include <TFT_ILI9163C.h>

GameUI::GameUI(GameOverlay* statusOverlay, Badge* badge) {
  this->statusOverlay = statusOverlay;
  this->badge = badge;
  Serial.printf("%x", this);
}

GameUI::~GameUI() {

}

void GameUI::draw(TFT_ILI9163C* tft, Theme * theme, uint16_t offsetX, uint16_t offsetY) {
  statusOverlay->draw(tft, theme, offsetX, offsetY);
    String message;
    switch(this->statusOverlay->getGameState()) {
      case GAME_STATE_SHARING_ACCESS: {
        message = "Awaiting more players to connect\n" + String(playerCount) + "/" + String(PLAYERS);
        break;
      }
      case GAME_STATE_HOST_AWAIT_START:
        message = "Press joystick to start";
        break;
      case GAME_STATE_RECEIVING_ACCESS:
        message = "Fetch Wifi information in IR sensor";
        break;
      case GAME_STATE_CONNECTING_WIFI:
        message = "Connecting Wifi";
        break;
      case GAME_STATE_CLIENT_AWAIT_START:
        message = "Waiting for Host to start";
        break;
      case GAME_STATE_RUNNING:
      case GAME_STATE_FINISH:
        message = lastPacketMessage;
        break;
    }

    tft->fillScreen(theme->backgroundColor);
    tft->setTextColor(theme->textColor);
    tft->setFont(&FreeSans7pt7b);
    tft->setTextSize(1);
    tft->setCursor(5, 30);
    tft->print(message);

    if (statusOverlay->getGameState() == GAME_STATE_RUNNING) {
      tft->drawLine(0, 65, _TFTWIDTH-1, 65, theme->foregroundColor);
      tft->setCursor(5, 80);
      tft->print(lastLogMessage);
    } else if (statusOverlay->getGameState() == GAME_STATE_FINISH) {
      tft->drawLine(0, 65, _TFTWIDTH-1, 65, theme->foregroundColor);
      tft->setCursor(5, 80);
      tft->print(finalMessage);
    }
    this->dirty = false;
}

void GameUI::handleJoin(ServerJoinAckPacket* packet) {
  dirty = true;
  this->statusOverlay->setPlayerId(packet->playerId);
}

void setLED(uint8_t led, uint32_t values) {
  uint8_t l = (uint8_t)((values & 0xFF000000) >> 24);
  uint8_t r = (uint8_t)((values & 0x00FF0000) >> 16);
  uint8_t g = (uint8_t)((values & 0x0000FF00) >> 8);
  uint8_t b = (uint8_t)((values & 0x000000FF));
  pixels.setPixelColor(led, pixels.Color(r, g, b));
}

void GameUI::resetLEDs() {
  setLED(0, 0);
  setLED(1, 0);
  setLED(2, 0);
  setLED(3, 0);
  pixels.show();
}

void GameUI::vib(uint16_t millis) {
  Serial.println(statusOverlay->getGameState());
  remainingVib = millis;
}

void GameUI::handleGameStart(ServerGameStartPacket* packet) {
  updateGameState(GAME_STATE_RUNNING);
  statusOverlay->setRound(packet->gameround);
  round = packet->gameround;
  lastPacketMessage = String(packet->text);
  lastLogMessage = "";
  finalMessage = "";
  setLED(0, packet->led1);
  setLED(1, packet->led2);
  setLED(2, packet->led3);
  setLED(3, packet->led4);
  pixels.show();
  remainingTime = packet->timeoutseconds * 1000;
}

void GameUI::doTime() {
  unsigned long last = lastTime;
  lastTime = millis();
  if (last == 0) {
    return;
  }

  unsigned long diff = lastTime - last;
  if (remainingTime > 0 && remainingTime > diff) {
    remainingTime = remainingTime - diff;
    statusOverlay->setRemainingTime(remainingTime);
  } else if (remainingTime > 0) {
    remainingTime = 0;
    statusOverlay->setRemainingTime(remainingTime);
  }

  if (remainingVib > 0 && remainingVib > diff) {
    remainingVib = remainingVib - diff;
    badge->setVibrator(true);
  } else if (remainingVib > 0) {
    remainingVib = 0;
    badge->setVibrator(false);
  }
}

void GameUI::handleGameOver(SeverGameOver* packet) {
  updateGameState(GAME_STATE_FINISH);
  finalMessage = "You survived\n" + String(round-1) + " rounds";
  if (packet->timeout) {
    lastPacketMessage = "Exploded:\nyou took to long";
  } else {
    lastPacketMessage = "Exploded:\nwrong input";
  }
  dirty = true;
  vib(500);
  setLED(0, 0xFFFF0000);
  setLED(1, 0xFFFF0000);
  setLED(2, 0xFFFF0000);
  setLED(3, 0xFFFF0000);
  pixels.show();
}

void GameUI::handleGameSuccess(ServerGameSuccess* packet) {
    updateGameState(GAME_STATE_FINISH);
    lastPacketMessage = "Congratulations";
    dirty = true;
    vib (300);
    setLED(0, 0xFF00FF00);
    setLED(1, 0xFF00FF00);
    setLED(2, 0xFF00FF00);
    setLED(3, 0xFF00FF00);
    pixels.show();
}
void GameUI::handleLogClientAction(ServerClientActionLogPacket* packet) {
  lastLogMessage = "Player " + String(packet->playerId) + " pushed ";
  switch (packet->stickdir) {
    case STICK_DIR_UP:
      lastLogMessage += "up";
      break;
    case STICK_DIR_DOWN:
      lastLogMessage += "down";
      break;
    case STICK_DIR_LEFT:
      lastLogMessage += "left";
      break;
    case STICK_DIR_RIGHT:
      lastLogMessage += "right";
      break;
  }
  vib(100);
  dirty = true;
}
