#include "GameServerProxy.h"

#include "defines.h"

bool GameServerProxy::begin() {


	ui->setOnPushDown(std::bind(&GameServerProxy::userAction, this, STICK_DIR_DOWN));
	ui->setOnPushUp(std::bind(&GameServerProxy::userAction, this, STICK_DIR_UP));
	ui->setOnPushLeft(std::bind(&GameServerProxy::userAction, this, STICK_DIR_LEFT));
	ui->setOnPushRight(std::bind(&GameServerProxy::userAction, this, STICK_DIR_RIGHT));

	if(!client.connect(ip, port)) {
		return false;
	}
	return true;
}

void GameServerProxy::userAction(uint8 direction) {
	ClientActionPacket p(direction, 0);

	p.writeToStream(client);
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
				ui->handleLogClientAction(&p);
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
				ui->handleGameSuccess(&p);
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