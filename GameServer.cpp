#include "GameServer.h"

void GameServer::doWork() {
	WiFiClient currentClient = server.available();
	if (currentClient && currentClient.connected())  {
		currentClients.push(currentClient);
	}
	// TODO: clients might connect in invalid states!
	// TODO: clients might disconnect
	for(std::list<WiFiClient>::iterator itr = currentClients.begin(); itr!= currentClients.end(); ++itr) {
		if(itr->available()) {
			int packetType = itr->read();
			switch(packetType) {
				case PACKET_CACTION:
				{
					Serial.printf("Client performed action!");
					ClientActionPacket p;
					p.readFromStream(*itr);

					handleClientInteraction(p, TODOUSUERID)
				}
			}
		}
	}
}

void GameServer::begin() {
	server.begin();
}

void GameServer::~GameServer() {
	server.close();
}