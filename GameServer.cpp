#include "GameServer.h"


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

void GameServer::startGame() {
	//CRITICAL: packets
	// broadcast, including user0
	// set timer
}

GameServer::~GameServer() {
	server.close();
}