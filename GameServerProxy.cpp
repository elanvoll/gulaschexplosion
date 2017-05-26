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
				ui->handleGameStart(&p);
				break;
			}
			case PACKET_SLOGACT: {
				ServerClientActionLogPacket p;
				p.readFromStream(client);

				Serial.println("UNIMPLEMENTED PACKET X! TODO!");
				break;
			}
			case PACKET_SGAMEOV: {
				SeverGameOver p;
				p.readFromStream(client);
				ui->handleGameOver(&p);
				break;
			}
			case PACKET_SGAMESUC: {
				ServerGameSuccess p;
				p.readFromStream(client);

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