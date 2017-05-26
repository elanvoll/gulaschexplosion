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
	round = readInt(st);
	text = st.readString();
	led0 = readInt(st);
	led1 = readInt(st);
	led2 = readInt(st);
	led3 = readInt(st);
	timeoutseconds = readInt(st);
}


