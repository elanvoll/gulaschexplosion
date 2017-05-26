#include "GameUI.h"
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans7pt7b.h>
#include <Fonts/Org_01.h>
#include <TFT_ILI9163C.h>

GameUI::GameUI() {

}

GameUI::~GameUI() {
  
}

void GameUI::draw(TFT_ILI9163C* tft, Theme * theme, uint16_t offsetX, uint16_t offsetY) {
    tft->fillScreen(theme->backgroundColor);
    tft->setTextColor(theme->textColor);
    tft->setFont(&FreeSans24pt7b);
    tft->setTextSize(1);
    tft->setCursor(5, 75);
    tft->print("Spiel");
    tft->setFont(&FreeSans9pt7b);
    this->dirty = false;
}

void GameUI::handleJoin(ServerJoinAckPacket* packet) {
  Serial.println("Joined");
  this->player = packet->playerId;
}