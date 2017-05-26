
#include <GPNBadge.hpp>

#include <BadgeUI.h>
#include <FS.h>

#include <rboot-api.h>

#include "rboot.h"
#include "rboot-api.h"


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define APP_NAME "Gulaschexplosion v0.1"



Badge badge;
WindowSystem* ui = new WindowSystem(&tft);
Menu * mainMenu = new Menu();

void setup() {
	badge.init();

  if (!SPIFFS.begin()) { //initialize the spiffs
    // TODO
  	Serial.print("Error init SPIFFS!");
  }

  badge.setBacklight(true);
  ui->root->setSub(APP_NAME);
  //ui->setOverlay(status);
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