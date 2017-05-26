#include "Packets.h"

int readInt(Stream& st) {
	int r = 0;
	for(int i=0; i<4;++i) {
		r <<= 8;
		r |= char(st.read());
	}
	return r;
}

bool ServerJoinAckPacket::readFromStream(Stream& st) {
	this->playerId = st.read();
}

bool ServerJoinAckPacket::writeToStream(Stream& st) {
	st.write(playerId);
}


bool ServerGameStartPacket::readFromStream(Stream& st) {
	gameround = readInt(st);
	text = st.readString();
	led1 = readInt(st);
	led2 = readInt(st);
	led3 = readInt(st);
	led4 = readInt(st);
	timeoutseconds = readInt(st);
}

bool ServerClientActionLogPacket::readFromStream(Stream& st) {
	stickdir = st.read();
	deviceorientation = st.read();
	playerid = st.read();
}

bool ClientActionPacket::readFromStream(Stream& st) {
	stickdir = st.read();
	deviceorientation = st.read();
}
