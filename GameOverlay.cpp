#include "GameOverlay.h"

#include <Fonts/Org_01.h>
#include "defines.h"

void GameOverlay::draw(TFT_ILI9163C* tft, Theme * theme, uint16_t offsetX, uint16_t offsetY) {
    tft->fillRect(0, 0,_TFTWIDTH, 11, theme->backgroundColor);
    tft->setTextColor(theme->textColor);
    tft->setCursor(0, 7);
    tft->setTextSize(1);
    tft->setFont(&Org_01);
    tft->print(APP_NAME);
    tft->setCursor(80, 7);
//    switch(this->gameState)
    //tft->printf("Bat: %d%%", std::max(std::min(int((bat-batCritical)/float(batFull-batCritical)*100), 100),0));
    tft->printf("Player %d", playerId);
    tft->drawLine(0, 11, _TFTWIDTH-1, 11, theme->foregroundColor);
    dirty = false;
}

uint16_t GameOverlay::getOffsetX(){
    return 0;
}

uint16_t GameOverlay::getOffsetY(){
    return 11;
}
