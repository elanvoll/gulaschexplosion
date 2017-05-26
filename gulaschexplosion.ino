
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

String shareString;

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
	char buffer[41];
	sprintf(buffer, "%s\t%s", ssid, randpw);
	shareString = String(buffer);
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
	ui->dispatchInput(badge.getJoystickState());
	ui->draw();
	switch(status->getGameState()) {
		case GAME_STATE_SHARING_ACCESS: {
			//IR Sharing
			Serial.print("Send wifi config: ");
			Serial.println(shareString);
			if (shareString.length() != 0) {
				badge.setGPIO(IR_EN, HIGH);
				Serial.println("NEC");
				uint32_t code = 0;
				uint8_t checksum = 0;
				for (int i = 0; i < shareString.length(); i++){
					ui->dispatchInput(badge.getJoystickState()); //Allow canceling
					checksum += shareString.charAt(i);
					code = code | shareString.charAt(i) << (i % 4)*8;
					if (i % 4 == 3) {
						irsend.sendNEC(code, 32);
						Serial.println(code, HEX);
						code = 0;
					}
				}
				if (code != 0) {
					irsend.sendNEC(code, 32);
					Serial.println(code, HEX);
				}
				Serial.print("Checksum: ");
				Serial.println(checksum); //224
				code = 0;
				code = checksum << 8 | 222;
				irsend.sendNEC(code, 32);
				badge.setGPIO(IR_EN, LOW);
			}
			break;
		}
		case GAME_STATE_RECEIVING_ACCESS:
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

	        if (millis() - on_time > 500) {
	          pixels.setPixelColor(1, pixels.Color(0, 0, 0));
	          pixels.show();
	        }

	        if (stringCompleted) {
	          uint8_t checksum = buf[1];
	          /*Serial.print("Checksum: ");
	          Serial.println(checksum);


	          Serial.print("Received Checksum: ");
	          Serial.println(checksumRec);*/
	          dataCompleted = true;

	          if(checksum == checksumRec) {
	            dataVerified = true;
	            pixels.setPixelColor(1, pixels.Color(0, 100, 0));
	          } else {
	            pixels.setPixelColor(1, pixels.Color(100, 0, 0));
	          }
	          pixels.show();
	          on_time = millis();
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
	        status->updateGameState(GAME_STATE_MAIN_MENU);
	        ui->closeCurrent();

	        size_t firstTab = received.indexOf("\t");

	        String ssid = received.substring(0, firstTab);
	        String psk = received.substring(firstTab + 1, received.length() - 1);

	        Serial.println("SSID: " + ssid);
	        Serial.println("PSK: " + psk);
	      }
	    }
	    badge.setGPIO(IR_EN, LOW);
	    Serial.println("Exiting receive mode");
			break;
	}
}
