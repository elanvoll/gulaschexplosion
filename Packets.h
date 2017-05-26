#ifndef _INC_PACKET_H

#define _INC_PACKET_H


#include <Stream.h>

int readInt(Stream& st);
void writeInt(Stream& st, uint32 in);
void writeString(Stream& st, String& s);

template <int TYPE>
struct GamePacket
{
	virtual bool writeToStream(Stream& st) {
		st.write(this->type);
		this->writeToStreamInternal(st);
	}
	virtual bool writeToStreamInternal(Stream & st) = 0;
	enum { type = TYPE};
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
		this->playerId = st.read();
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
	ServerGameStartPacket(int gameround, String text, uint32 led1, uint32 led2, uint32 led3, uint32 led4, uint32 timeoutseconds) : 
	gameround(gameround), text(text), led1(led1), led2(led2), led3(led3), led4(led4), timeoutseconds(timeoutseconds) {

	}
	bool readFromStream(Stream& st) {
		gameround = readInt(st);
		text = st.readString();
		led1 = readInt(st);
		led2 = readInt(st);
		led3 = readInt(st);
		led4 = readInt(st);
		timeoutseconds = readInt(st);
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
	
	int gameround;
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
		stickdir = st.read();
		deviceorientation = st.read();
		playerid = st.read();
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
		stickdir = st.read();
		deviceorientation = st.read();
	}
	bool writeToStreamInternal(Stream& st) {
		st.write(stickdir);
		st.write(deviceorientation);
	}
	uint8 stickdir;
	uint8 deviceorientation;
};


#endif