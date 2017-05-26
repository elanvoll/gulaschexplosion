#include "Packets.h"

uint32 readUInt32(Stream& st) {
	uint32 ret;
	st.readBytes((char*)&ret, 4);
	return ret;
}


void writeInt(Stream& st, uint32 in) {
	st.write((char*)&in, 4);
}

void writeString(Stream& st, String& s) {
	st.write(s.c_str(), s.length()+1);
}