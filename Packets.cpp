#include "Packets.h"

bool ServerJoinAckPacket::readFromStream(Stream& st) {
	this->playerId = st.read();
}


bool ServerGameStartPacket::readFromStream(Stream& st) {
	int round;
	String text;
	int led0;
	int led1;
	int led2;
	int led3;
	int timeoutseconds;
}