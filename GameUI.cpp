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
      case GAME_STATE_RUNNING:
        message = "Lets play";
        break;
      case GAME_STATE_RECEIVING_ACCESS:
        message = "Warte auf IR";
        break;
      case GAME_STATE_CLIENT_AWAIT_START:
        message = "Waiting for Host to start";
        break;
    }

    tft->fillScreen(theme->backgroundColor);
    tft->setTextColor(theme->textColor);
    tft->setFont(&FreeSans9pt7b);
    tft->setTextSize(1);
    tft->setCursor(5, 30);
    tft->print(message);
    this->dirty = false;
}

void GameUI::handleJoin(ServerJoinAckPacket* packet) {
  Serial.println(packet->playerId);
  this->statusOverlay->setPlayerId(packet->playerId);
}

void GameUI::handleGameStart(ServerGameStartPacket* packet) {
 // TODO: Implement me!
  //throw "Implement me plz";
}