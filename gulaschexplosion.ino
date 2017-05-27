
#include <GPNBadge.hpp>

#include <BadgeUI.h>
#include "GameUI.h"
#include <FS.h>

#include <rboot-api.h>

#include "rboot.h"
#include "rboot-api.h"

#include <stdint.h>

#include "GameOverlay.h"
#include "GameServer.h"
#include "GameServerProxy.h"

#include "defines.h"

Badge badge;
WindowSystem* ui = new WindowSystem(&tft);
Menu * mainMenu = new Menu();
GameOverlay * status = new GameOverlay(GAME_STATE_MAIN_MENU);
uint8 state = 0;

AbstractGameServer* gameServer = NULL;
GameUI* gameUi = NULL;

int8 player_id = -1;
// Information the Host sends to all clients
// encoded in format SSID\tPSK\tGAME_VERSION\n
String shareString;

bool isHost() {
  return player_id == PLAYER_ID_HOST;
}

void hostgame() {
	WiFi.disconnect();
	delay(100);
	WiFi.mode(WIFI_AP_STA);
	WiFi.softAPConfig(HOST_IP, HOST_IP, IPAddress(255, 255, 255, 0));
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
	char buffer[41];
	sprintf(buffer, "%s\t%s\t%s\n", ssid, randpw, APP_VERSION);
	shareString = String(buffer);

  gameServer = new GameServer(GAME_TCP_PORT, gameUi);
  ((GameServer*)gameServer)->begin();
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
    gameUi = new GameUI(status, &badge);
    gameUi->updateGameState(GAME_STATE_SHARING_ACCESS);
    ui->open(gameUi);
    hostgame();
  }));

  mainMenu->addMenuItem(new MenuItem("Join game", []() {
		gameUi = new GameUI(status, &badge);
    gameUi->updateGameState(GAME_STATE_RECEIVING_ACCESS);
    ui->open(gameUi);
  }));

  mainMenu->addMenuItem(new MenuItem("Du Success", []() {
    gameUi = new GameUI(status, &badge);
    ui->open(gameUi);
    gameUi->updateGameState(GAME_STATE_RUNNING);
    ServerGameSuccess s = ServerGameSuccess();
    gameUi->handleGameSuccess(&s);
    gameUi->setOnPushEnter([]() {
      gameUi->resetLEDs();
      ui->closeCurrent();
    });
  }));

  mainMenu->addMenuItem(new MenuItem("Du Game Over", []() {
    gameUi = new GameUI(status, &badge);
    ui->open(gameUi);
    gameUi->updateGameState(GAME_STATE_RUNNING);
    SeverGameOver s = SeverGameOver(true, 1, 2, 3);
    gameUi->handleGameOver(&s);
    gameUi->setOnPushEnter([]() {
      gameUi->resetLEDs();
      ui->closeCurrent();
    });
  }));

  mainMenu->addMenuItem(new MenuItem("Dumnmy Start", []() {
    gameUi = new GameUI(status, &badge);
    ui->open(gameUi);
    ServerGameStartPacket s = ServerGameStartPacket(1, "Player 2 must not press any button", 0x222222, 0x44004400, 0x55550000, 0x22222222, 5);
    ServerClientActionLogPacket l = ServerClientActionLogPacket(STICK_DIR_UP, 0, 2);
    ServerGameSuccess su = ServerGameSuccess();
    gameUi->handleGameStart(&s);
    gameUi->handleLogClientAction(&l);
    gameUi->handleGameSuccess(&su);
    gameUi->setOnPushEnter([]() {
      gameUi->resetLEDs();
      ui->closeCurrent();
    });
  }));
  ui->open(mainMenu);
}

void loop() {
	ui->dispatchInput(badge.getJoystickState());
	ui->draw();
  if (gameUi != NULL) {
    gameUi->doTime();
  }
	switch(status->getGameState()) {
		case GAME_STATE_RECEIVING_ACCESS:
			receiveGameInformation();
      break;
    case GAME_STATE_SHARING_ACCESS:
      sendGameInformation();
       // no break by intention!
    case GAME_STATE_HOST_AWAIT_START:
    case GAME_STATE_CLIENT_AWAIT_START:
    case GAME_STATE_RUNNING:
    case GAME_STATE_FINISH:
      if (gameServer != NULL) {
        gameServer->doWork();
      }
	}
}

void sendGameInformation() {
	//IR Sharing
	Serial.print("Send wifi config: ");
	Serial.println(shareString);
	if (shareString.length() != 0) {
		badge.setGPIO(IR_EN, HIGH);
		// Serial.println("NEC");
		uint32_t code = 0;
		uint8_t checksum = 0;
		for (int i = 0; i < shareString.length(); i++){
			ui->dispatchInput(badge.getJoystickState()); //Allow canceling
			checksum += shareString.charAt(i);
			code = code | shareString.charAt(i) << (i % 4)*8;
			if (i % 4 == 3) {
				irsend.sendNEC(code, 32);
				// Serial.println(code, HEX);
				code = 0;
			}
		}
		if (code != 0) {
			irsend.sendNEC(code, 32);
			// Serial.println(code, HEX);
		}
		// Serial.print("Checksum: ");
		// Serial.println(checksum); //224
		code = 0;
		code = checksum << 8 | 222;
		irsend.sendNEC(code, 32);
		badge.setGPIO(IR_EN, LOW);
	}
}

void receiveGameInformation() {
	// do IR recieve
	badge.setGPIO(IR_EN, HIGH);
	Serial.println("Entering receive mode");
	String received = "";
	bool stringCompleted = false;
	bool dataCompleted = false;
	bool dataVerified = false;
	uint8_t checksumRec = 0;

	while (!dataCompleted && status->getGameState() == GAME_STATE_RECEIVING_ACCESS) {
		int on_time = 0;
		decode_results  results;        // Somewhere to store the results
		if (irrecv.decode(&results)) {
			if (results.overflow) {
				Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWBUF");
				return;
			}

			char * buf = reinterpret_cast<char*>(&results.value);

			if (stringCompleted) {
				uint8_t checksum = buf[1];
				dataCompleted = true;

				if(checksum == checksumRec) {
					dataVerified = true;
				}
			}
			else {
				Serial.println(buf);
				received += String(buf[0]) + buf[1] + buf[2] + buf[3];
				checksumRec += buf[0] + buf[1] + buf[2] + buf[3];
				if (buf[0] == '\n' || buf[1] == '\n' || buf[2] == '\n' || buf[3] == '\n') {
					stringCompleted = true;
				}
			}
			irrecv.resume();              // Prepare for the next value
		}
		delay(0);
		ui->dispatchInput(badge.getJoystickState()); //Allow canceling
	}

	if (dataCompleted == true){
		if ( dataVerified == true) {
			Serial.print(received);

			size_t firstTab = received.indexOf("\t");
			size_t secondTab = received.indexOf("\t", firstTab + 1);

			String ssid = received.substring(0, firstTab);
			String psk = received.substring(firstTab + 1, secondTab);
			String version = received.substring(secondTab + 1, received.length() - 1);
      version.trim();

			Serial.println("SSID: " + ssid);
			Serial.println("PSK: " + psk);
			Serial.println("Version: " + version);

			if (strcmp(version.c_str(), APP_VERSION) != 0) {
				showVersionErrorScreen(version);
			} else {
				connectToWifi(ssid, psk);
			}
		}
	}
	badge.setGPIO(IR_EN, LOW);
	Serial.println("Exiting receive mode");
}

void connectToWifi(String ssid, String psk) {
  gameUi->updateGameState(GAME_STATE_CONNECTING_WIFI);
  ui->draw();
	WiFi.begin(ssid.c_str(), psk.c_str()); // quick fix.. chnage in js
  Serial.printf("Trying connect to %s...\n", ssid.c_str());
  int wStat = WiFi.status();
  int ledVal = 0;
  bool up = true;
  while (wStat != WL_CONNECTED) {
    if (wStat == WL_CONNECT_FAILED) {
      break;
    }
    pixels.setPixelColor(1, pixels.Color(ledVal, 0, ledVal));
    pixels.setPixelColor(2, pixels.Color(0, ledVal, ledVal));
    pixels.setPixelColor(3, pixels.Color(ledVal, ledVal, ledVal));
    pixels.setPixelColor(0, pixels.Color(0, 0, ledVal));
    pixels.show();
    if (ledVal == 100) {
      up = false;
    }
    if (ledVal == 0) {
      up = true;
    }
    if (up) {
      ledVal++;
    } else {
      ledVal--;
    }
    delay(10);
    wStat = WiFi.status();
    Serial.println(wStat);
  }
  pixels.setPixelColor(1, pixels.Color(0, 0, 0));
  pixels.setPixelColor(2, pixels.Color(0, 0, 0));
  pixels.setPixelColor(3, pixels.Color(0, 0, 0));
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();
  if (wStat == WL_CONNECTED) {
    Serial.println("Success");
		Serial.println("IP: " + WiFi.localIP().toString());
    gameUi->updateGameState(GAME_STATE_CLIENT_AWAIT_START);
		gameServer = new GameServerProxy(HOST_IP, GAME_TCP_PORT, gameUi);
    ((GameServerProxy*)gameServer)->begin();
  } else {
    Serial.println("Fail");
    showWifiConnectionError();
  }
}

void showWifiConnectionError() {
  ClosableTextDisplay * wifiErrorScreen = new ClosableTextDisplay();
	wifiErrorScreen->setText("Cannot establish wifi connection.");
	wifiErrorScreen->setOnClose([]() {
		gameUi->updateGameState(GAME_STATE_MAIN_MENU);
		ui->closeCurrent();
    ui->open(mainMenu);
	});
  ui->closeCurrent();
	ui->open(wifiErrorScreen);
}

void showVersionErrorScreen(String version) {
	ClosableTextDisplay * versionErrorScreen = new ClosableTextDisplay();
	versionErrorScreen->setText("Server Version " + version + " is incompatible with client Version " + APP_VERSION);
	versionErrorScreen->setOnClose([]() {
		gameUi->updateGameState(GAME_STATE_MAIN_MENU);
		ui->closeCurrent();
	});
	ui->open(versionErrorScreen);
}
