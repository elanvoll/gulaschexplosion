#include "Packets.h"

int readInt(Stream& st) {
	int r = 0;
	for(int i=0; i<4;++i) {
		r <<= 8;
		r |= char(st.read());
	}
	return r;
}


void writeInt(Stream& st, uint32 in) {
	for (int i=0; i<4; ++i) {
		uint8 o = in & 0x000000FF;
		st.write(o);
		in >>= 8;
	}
}

void writeString(Stream& st, String& s) {
	st.write(s.c_str());
}