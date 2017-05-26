#include "GameServer.h"

#include "defines.h"

void GameServer::handleClientInteraction(ClientActionPacket& p, uint8 userid) {
	// TODO
}

void GameServer::doWork() {
	WiFiClient currentClient = server.available();
	if (currentClient && currentClient.connected())  {
		currentClients.push_back(currentClient);

		// CRITICAL: alert someone?!
		uint8 playerid = currentClients.size();
		ServerJoinAckPacket p;
		p.playerId = playerid;
		p.writeToStream(currentClient);
	}
	// TODO: clients might connect in invalid states!
	// TODO: clients might disconnect
	int userid = 0;
	for(std::list<WiFiClient>::iterator itr = currentClients.begin(); itr!= currentClients.end(); ++itr) {
		userid ++;
		if(itr->available()) {
			int packetType = itr->read();
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
	server.begin();
	ServerJoinAckPacket p;
	p.playerId = 0;
	ui->handleJoin(&p);
	ui->setOnGameStart(std::bind(&GameServer::startGame, this));
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

void GameServer::startGame() {
	//CRITICAL: packets
	// broadcast, including user0
	// set timer
	GameRound* r = generateGameRound();
	//ServerGameStartPacket
}

GameServer::~GameServer() {
	server.close();
}
