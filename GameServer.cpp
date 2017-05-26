#include "GameServer.h"

#include "defines.h"

void GameServer::handleClientInteraction(ClientActionPacket& p, uint8 userid) {
	// TODO
}

void GameServer::doWork() {
	WiFiClient currentClient = server.available();
	if (currentClient && currentClient.connected())  {
		Serial.println("User joined");
		currentClients.push_back(currentClient);

		uint8 playerid = currentClients.size();
		ServerJoinAckPacket p;
		p.playerId = playerid;
		p.writeToStream(currentClient);
		ui->serverNewUser(playerid);
	}
	// TODO: clients might connect in invalid states!
	// TODO: clients might disconnect
	int userid = 0;
	for(std::list<WiFiClient>::iterator itr = currentClients.begin(); itr!= currentClients.end(); ++itr) {
		userid ++;
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
}

void GameServer::begin() {
	ui->setOnGameStart(std::bind(&GameServer::startGame, this));
	server.begin();
	ServerJoinAckPacket p;
	p.playerId = 0;
	ui->handleJoin(&p);
	ui->serverNewUser(0);
}

GameRound* GameServer::generateGameRound() {
	std::list<ServerGameStartPacket> instructions;
	instructions.push_back(ServerGameStartPacket(1, "Player 2 must not press any button", 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 20));
	instructions.push_back(ServerGameStartPacket(1, "A player has to select green", 0xFF00FF00, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 20));
	instructions.push_back(ServerGameStartPacket(1, "If there is red, select it first", 0xFF00FF00, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 20));
	// richtig: player 1 geht zu led1 = oben
	std::list<ClientActionPacket> correctSeq;
	correctSeq.push_back(ClientActionPacket(STICK_DIR_UP, 0));

	switch(currentRound) {
		case 1:
		return new GameRound(instructions, correctSeq);
	}
}

// clicked by user
void GameServer::startGame() {

	GameRound* r = generateGameRound();
	Serial.println("host clicked start");

	std::list<ServerGameStartPacket>::iterator instrItr = r->instructions.begin();
	// user0 gets special treatment
	ui->handleGameStart(&*instrItr);

	++instrItr;

	std::list<WiFiClient>::iterator userItr = currentClients.begin();
	while(instrItr != r->instructions.end() && userItr != currentClients.end()) {
		instrItr->writeToStream(*userItr);
		++userItr;
		++instrItr;
	}
	// CRITICAL! set timer
}

GameServer::~GameServer() {
	server.close();
}
