#include "Packets.h"

uint32 readUInt32(Stream& st) {
	uint32 ret;
	st.readBytes((char*)&ret, 4);
}


void writeInt(Stream& st, uint32 in) {
	for (int i=0; i<4; ++i) {
		uint8 o = in & 0x000000FF;
		st.write(o);
		in >>= 8;
	}
}

void writeString(Stream& st, String& s) {
	st.write(s.c_str(), s.length()+1);
}