#include "GameServerProxy.h"

#include "defines.h"

bool GameServerProxy::begin() {
	if(!client.connect(ip, port)) {
		return false;
	}
	return true;
}

void GameServerProxy::doWork() {
	while(client.connected() && client.available()) {
		int packetType = client.read();
		Serial.print("Got packetType:");
		Serial.println(packetType);

		switch(packetType) {
			case PACKET_SJOIN: {
				ServerJoinAckPacket p;
				p.readFromStream(client);
				ui->handleJoin(&p);
			}
			case PACKET_SSTART: {
				ServerGameStartPacket p;
				p.readFromStream(client);
			}
			case PACKET_SLOGACT: {

			}
			case PACKET_SGAMEOV: {

			}
			case PACKET_SGAMESUC: {

			}
			default:
			Serial.println("UNKNOWN PACKET! PANIC!");
		}
	}
}

GameServerProxy::~GameServerProxy() {
	client.stop();
}