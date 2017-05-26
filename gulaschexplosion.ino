
#include <GPNBadge.hpp>

#include <BadgeUI.h>
#include <FS.h>

#include <rboot-api.h>

#include "rboot.h"
#include "rboot-api.h"

#include <stdint.h>

#include "GameOverlay.h"

#include "defines.h"

Badge badge;
WindowSystem* ui = new WindowSystem(&tft);
Menu * mainMenu = new Menu();
GameOverlay * status = new GameOverlay(GAME_STATE_MAIN_MENU);

void setup() {
	badge.init();

  if (!SPIFFS.begin()) { //initialize the spiffs
    // TODO
  	Serial.print("Error init SPIFFS!");
  }

  badge.setBacklight(true);
  ui->root->setSub(APP_NAME);
  ui->setOverlay(status);
  ui->draw();

  badge.setGPIO(IR_EN, LOW);

  badge.setAnalogMUX(MUX_JOY);

  rboot_config rboot_config = rboot_get_config();
  Serial.print("I am in Slot ");
  Serial.println(rboot_config.current_rom);

  SPIFFS.begin();
  File f = SPIFFS.open("/rom" + String(rboot_config.current_rom), "w");
  f.println(APP_NAME);

  mainMenu->addMenuItem(new MenuItem("Host game", []() {
  	//
  }));

  mainMenu->addMenuItem(new MenuItem("Join game", []() {
  	//
  }));
  ui->open(mainMenu);
}

void loop() {
	ui->dispatchInput(badge.getJoystickState());
	ui->draw();
}