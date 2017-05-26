
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
uint8 state = 0;


int8 player_id = -1;

bool isHost() {
  return player_id == PLAYER_ID_HOST;
}

void hostgame() {
	WiFi.disconnect();
	delay(100);
	WiFi.mode(WIFI_AP_STA);
	WiFi.softAPConfig(IPAddress (10, 0, 0, 1), IPAddress(10, 0, 0, 1), IPAddress(255, 255, 255, 0));
	char randpw[20];
	char ssid[20];
	int seed = millis();
	for (int i = 0; i < 15; ++i) {
		seed += badge.getBatLvl();
		seed += badge.getLDRLvl();
	}
	randomSeed(seed);
	sprintf(randpw, "%d", random(10000000, 100000000)); // TODO richtig machen
	sprintf(ssid, "ESP%d", ESP.getChipId());
	WiFi.softAP(ssid, randpw);
}

bool joingame(const char* ssid, const char* pw) {
  WiFi.mode(WIFI_STA);
  delay(30);
  
  // might not be requried...
  ssid = strdup(ssid);

  Serial.printf("Connecting ti wifi '%s' with password '%s'...\n", ssid, pw);
  WiFi.begin(ssid, pw);
  return WiFi.status() == WL_CONNECTED;

}

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
    status->updateGameState(GAME_STATE_SHARING_ACCESS);    
    hostgame();
    ClosableTextDisplay* host_screen = new ClosableTextDisplay();
    host_screen->setText(STATE_HOST_MESSAGE);
    host_screen->setOnClose([]() {
      status->updateGameState(GAME_STATE_MAIN_MENU);
      ui->closeCurrent();
    });
    ui->open(host_screen);
  }));

  mainMenu->addMenuItem(new MenuItem("Join game", []() {
		status->updateGameState(GAME_STATE_RECEIVING_ACCESS);
		ClosableTextDisplay* join_screen = new ClosableTextDisplay();
		join_screen->setText(STATE_JOIN_MESSAGE);
		join_screen->setOnClose([]() {
			status->updateGameState(GAME_STATE_MAIN_MENU);
      ui->closeCurrent();
    });
    ui->open(join_screen);

    bool joinedgame = joingame("test", "test");
    Serial.println("joined game:");
    Serial.println(joinedgame);
  }));
  ui->open(mainMenu);
}

void loop() {
	switch(status->getGameState()) {
		case GAME_STATE_SHARING_ACCESS:
			// do IR Send
			Serial.println("send ir");
			break;
		case GAME_STATE_RECEIVING_ACCESS:
			// do IR recieve
			Serial.println("recieve ir");
			break;
	}
	ui->dispatchInput(badge.getJoystickState());
	ui->draw();
}
