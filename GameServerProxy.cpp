#include "GameServerProxy.h"

#include "defines.h"

bool GameServerProxy::begin() {
	if(!client.connect(HOST_IP, GAME_TCP_PORT)) {
		return false;
	}
	return true;
}

void GameServerProxy::doWork() {
	while(client.connected() && client.available()) {
		int packetType = client.read();
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
		}
	}
}

GameServerProxy::~GameServerProxy() {
	client.stop();
}