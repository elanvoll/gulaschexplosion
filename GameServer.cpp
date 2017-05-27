#include "GameServer.h"

#include "defines.h"

#include "GameRoundGenerator.h"


void GameServer::handleClientInteraction(ClientActionPacket& p, uint8 userid) {
	if (serverState != GAME_SERVER_STATE_RUNNING) {
		Serial.println("got client interaction while game is not running");
		return;
	}
	ServerClientActionLogPacket p2(p.stickdir, 0, userid);
	broadcast(&p2);
	ui->handleLogClientAction(&p2);

	// correct one?
	if(correctSequence.size() == 0) {
		Serial.println("Got client interaction although done?!");
		return;
	}

	ServerClientActionLogPacket correctAction = *correctSequence.begin();
	correctSequence.pop_front();
	if(userid != correctAction.playerId || p.stickdir != correctAction.stickdir ) {
		serverState = GAME_SERVER_STATE_READY;
		nextGameRound = 1;
		SeverGameOver gov(false, correctAction.playerId, correctAction.stickdir, correctAction.deviceorientation);
		ui->handleGameOver(&gov);
		broadcast(&gov);
	} else if (correctSequence.size() == 0) {
		Serial.println("game successfully ended");
		serverState = GAME_SERVER_STATE_READY;
		ServerGameSuccess p;
		nextGameRound++;

		ui->handleGameSuccess(&p);
		broadcast(&p);
		timeoutms = 0;
	}
}

void GameServer::doWork() {
	WiFiClient currentClient = server.available();
	if (currentClient && currentClient.connected())  {
		currentClient.setNoDelay(true);
		Serial.println("User joined");
		if(serverState != GAME_SERVER_STATE_LISTEN) {
			Serial.println("Got new player in wrong state");
			currentClient.stop();
		} else {
			currentClients.push_back(currentClient);

			// +1: local player is not in currentClients
			if(currentClients.size()+1 == PLAYERS) {
				serverState = GAME_SERVER_STATE_READY;
			}
			uint8 playerid = currentClients.size();
			ServerJoinAckPacket p;
			p.playerId = playerid;
			p.writeToStream(currentClient);
			ui->serverNewUser(playerid);
		}
	}
	// TODO: clients might disconnect
	int userid = 0;
	for(std::list<WiFiClient>::iterator itr = currentClients.begin(); itr!= currentClients.end(); ++itr) {
		userid ++;
		if(!itr->connected()) {
			Serial.printf("WARN: playerid=%d is disconnected", userid);
		}
		if(itr->available()) {
			Serial.print("Got data from client: ");
			int packetType = itr->read();
			Serial.println(packetType);
			switch(packetType) {
				case PACKET_CACTION:
				{
					Serial.printf("Client performed action!");
					ClientActionPacket p;
					p.readFromStream(*itr);

					handleClientInteraction(p, userid);
				}
			}
		}
	}

	if (serverState == GAME_SERVER_STATE_RUNNING && timeoutms < millis()) {
		serverState = GAME_SERVER_STATE_READY;
		ServerClientActionLogPacket* p = &*correctSequence.begin();
		SeverGameOver gov(true, p->playerId, p->stickdir, p->deviceorientation);

		ui->handleGameOver(&gov);
		broadcast(&gov);
	}
}

void GameServer::broadcast(BasePacket *p) {
	for(std::list<WiFiClient>::iterator itr = currentClients.begin(); itr!= currentClients.end(); ++itr) {
		p->writeToStream(*itr);
	}
}

void GameServer::userActionHost(uint8 direction) {
	ClientActionPacket p(direction, 0);

	this->handleClientInteraction(p, 0);
}

void GameServer::begin() {
	ui->setOnGameStart(std::bind(&GameServer::startGame, this));
	ui->setOnContinue(std::bind(&GameServer::startGame, this));

	ui->setOnPushDown(std::bind(&GameServer::userActionHost, this, STICK_DIR_DOWN));
	ui->setOnPushUp(std::bind(&GameServer::userActionHost, this, STICK_DIR_UP));
	ui->setOnPushLeft(std::bind(&GameServer::userActionHost, this, STICK_DIR_LEFT));
	ui->setOnPushRight(std::bind(&GameServer::userActionHost, this, STICK_DIR_RIGHT));

	server.begin();
	ServerJoinAckPacket p;
	p.playerId = 0;
	ui->handleJoin(&p);
	ui->serverNewUser(0);
}

GameRound* GameServer::generateGameRound() {
	return GameRoundGenerator::newRound(this->nextGameRound);
}

// clicked by user
void GameServer::startGame() {
	if(serverState != GAME_SERVER_STATE_READY) {
		Serial.println("got start althought not ready");
		return;
	}
	GameRound* r = generateGameRound();
	Serial.println("host clicked start");

	this->correctSequence = r->correctSeq;

	std::list<ServerGameStartPacket>::iterator instrItr = r->instructions.begin();

	serverState = GAME_SERVER_STATE_RUNNING;
	this->timeoutms = instrItr->timeoutseconds*1000 + TIMEOUT_PROP_DELAY_MS + millis();
	// user0 gets special treatment
	ui->handleGameStart(&*instrItr);

	++instrItr;

	std::list<WiFiClient>::iterator userItr = currentClients.begin();
	while(instrItr != r->instructions.end() && userItr != currentClients.end()) {
		Serial.println("Send gamestart to a player");
		instrItr->writeToStream(*userItr);
		++userItr;
		++instrItr;
	}
	if(instrItr != r->instructions.end() || userItr != currentClients.end())
		Serial.println("ERROR: instruction count doesn't match user number");
	delete r;
}

GameServer::~GameServer() {
	server.close();
}
