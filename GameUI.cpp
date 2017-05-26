#include "GameUI.h"
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans7pt7b.h>
#include <Fonts/Org_01.h>
#include <TFT_ILI9163C.h>

GameUI::GameUI(GameOverlay* statusOverlay) {
  this->statusOverlay = statusOverlay;
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
        message = "Warte auf IR";
        break;
      case GAME_STATE_CLIENT_AWAIT_START:
        message = "Waiting for Host to start";
        break;
      case GAME_STATE_RUNNING:
        message = lastPacketMessage;
        break;
    }

    tft->fillScreen(theme->backgroundColor);
    tft->setTextColor(theme->textColor);
    tft->setFont(&FreeSans9pt7b);
    tft->setTextSize(1);
    tft->setCursor(5, 30);
    tft->print(message);
    if (statusOverlay->getGameState() == GAME_STATE_RUNNING) {
      tft->setCursor(5, 90);
      char time[10];
      sprintf(time, "%.3f s", remainingTime / 1000.);
      tft->print(time);
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
  Serial.printf("%d/%d/%d/%d\n", l, r, g, b);
  pixels.setPixelColor(led, pixels.Color(r, g, b));
}

void GameUI::handleGameStart(ServerGameStartPacket* packet) {
  Serial.println("Start");
  updateGameState(GAME_STATE_RUNNING);
  lastPacketMessage = String(packet->text);
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
    dirty = true;
  } else if (remainingTime > 0) {
    remainingTime = 0;
    dirty = true;
  }
}
