#ifndef _INC_PACKET_H

#define _INC_PACKET_H


#include <Stream.h>

#include <HardwareSerial.h>

uint32 readUInt32(Stream& st);

void writeInt(Stream& st, uint32 in);
void writeString(Stream& st, String& s);

template <int TYPE>
struct GamePacket
{
	virtual bool writeToStream(Stream& st) {
		st.write(uint8(TYPE));
		this->writeToStreamInternal(st);
	}
	virtual bool writeToStreamInternal(Stream & st) = 0;
};



/*

future idea: pulsing vibration as indicator ("press if player x' device morses SOS")

Flow example:
 ServerJoinAckPacket(playerid = 2 [Range: 1..3?5?])
 ServerGameStartPacket(gameround, text = "player 3 shall press left", led1 = 0xffffffff, led2, led3, led4, timeoutseconds - delta)
 ClientActionPacket(stickdir = 2, device orientation)
 ServerClientActionLogPacket(playerid, stickdir, deviceorientation)
  Unterscheidung:
   SeverGameOver(timeout true/false, correctplayerid, correctstickdir, correctdeviceor)
  Oder
   SeverGameSuccess!()
  Host klickt auf next gameround:
   ServerGameStartPacket wieder
*/

enum packet_types_t
{
	PACKET_SJOIN = 0,
	PACKET_SSTART,
	PACKET_CACTION,
	PACKET_SLOGACT,
	PACKET_SGAMEOV,
	PACKET_SGAMESUC
};

struct ServerJoinAckPacket : GamePacket<PACKET_SJOIN>
{	
	ServerJoinAckPacket() {

	}
	
	bool readFromStream(Stream& st) {
		st.readBytes(&playerId, 1);
		Serial.printf("ServerJoinAckPacket - read userid %d\n", playerId);
	}
	bool writeToStreamInternal(Stream& st) {
		st.write(playerId);
	}
	uint8 playerId;
};

struct ServerGameStartPacket : GamePacket<PACKET_SSTART>
{	
	ServerGameStartPacket() {

	}
	ServerGameStartPacket(uint32 gameround, String text, uint32 led1, uint32 led2, uint32 led3, uint32 led4, uint32 timeoutseconds) : 
	gameround(gameround), text(text), led1(led1), led2(led2), led3(led3), led4(led4), timeoutseconds(timeoutseconds) {

	}
	bool readFromStream(Stream& st) {
		gameround = readUInt32(st);
		text = st.readString();
		led1 = readUInt32(st);
		led2 = readUInt32(st);
		led3 = readUInt32(st);
		led4 = readUInt32(st);
		timeoutseconds = readUInt32(st);
	}
	bool writeToStreamInternal(Stream& st) {
		writeInt(st, gameround);
		writeString(st, text);
		writeInt(st, led1);
		writeInt(st, led2);
		writeInt(st, led3);
		writeInt(st, led4);
		writeInt(st, timeoutseconds);
	}
	
	uint32 gameround;
	String text;
	uint32 led1;
	uint32 led2;
	uint32 led3;
	uint32 led4;
	uint32 timeoutseconds;
};

struct ServerClientActionLogPacket : GamePacket<PACKET_SLOGACT>
{	
	ServerClientActionLogPacket() {

	}
	bool readFromStream(Stream& st) {
		st.readBytes((char*)&stickdir, 1);
		st.readBytes((char*)&deviceorientation, 1);
		st.readBytes((char*)&playerid, 1);
	}
	bool writeToStreamInternal(Stream& st) {
		st.write(stickdir);
		st.write(deviceorientation);
		st.write(playerid);
	}
	int stickdir;
	int deviceorientation;
	uint8 playerid;
};

struct ClientActionPacket : GamePacket<PACKET_CACTION> {
	ClientActionPacket(uint8 stickdir, uint8 deviceorientation) :
		stickdir(stickdir), deviceorientation(deviceorientation) {

	}

	ClientActionPacket(): stickdir(0), deviceorientation(0) {

	}
	bool readFromStream(Stream& st) {
		st.readBytes((char*)&stickdir, 1);
		st.readBytes((char*)&deviceorientation, 1);
	}
	bool writeToStreamInternal(Stream& st) {
		st.write(stickdir);
		st.write(deviceorientation);
	}
	uint8 stickdir;
	uint8 deviceorientation;
};


#endif