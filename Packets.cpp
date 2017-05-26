#include "Packets.h"

bool ServerJoinAckPacket::readFromStream(Stream& st) {
	this->playerId = st.read();
}