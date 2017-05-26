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
				break;
			}
			case PACKET_SSTART: {
				ServerGameStartPacket p;
				p.readFromStream(client);
				break;
			}
			case PACKET_SLOGACT: {
				Serial.println("UNIMPLEMENTED PACKET X! TODO!");
				break;
			}
			case PACKET_SGAMEOV: {
				Serial.println("UNIMPLEMENTED PACKET Y! TODO!");
				break;
			}
			case PACKET_SGAMESUC: {
				Serial.println("UNIMPLEMENTED PACKET Z! TODO!");
				break;
			}
			default:
			Serial.println("UNKNOWN PACKET! PANIC!");
		}
	}
}

GameServerProxy::~GameServerProxy() {
	client.stop();
}