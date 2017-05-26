#ifndef _INC_PACKET_H

#define _INC_PACKET_H


#include <Stream.h>


struct GamePacket
{
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

struct ServerJoinAckPacket : GamePacket
{	
	ServerJoinAckPacket() {

	}
	
	bool readFromStream(Stream& st);
	bool writeToStream(Stream& st);
	uint8 playerId;
};

struct ServerGameStartPacket : GamePacket
{	
	ServerGameStartPacket() {

	}
	ServerGameStartPacket(int gameround, String text, uint32 led1, uint32 led2, uint32 led3, uint32 led4, uint32 timeoutseconds) : 
	gameround(gameround), text(text), led1(led1), led2(led2), led3(led3), led4(led4), timeoutseconds(timeoutseconds) {

	}
	bool readFromStream(Stream& st);
	bool writeToStream(Stream& st);
	
	int gameround;
	String text;
	uint32 led1;
	uint32 led2;
	uint32 led3;
	uint32 led4;
	uint32 timeoutseconds;
};

struct ServerClientActionLogPacket : GamePacket
{	
	ServerClientActionLogPacket() {

	}
	bool readFromStream(Stream& st);
	bool writeToStream(Stream& st);
	int stickdir;
	int deviceorientation;
	uint8 playerid;
};

struct ClientActionPacket : GamePacket {
	ClientActionPacket(uint8 stickdir, uint8 deviceorientation) :
		stickdir(stickdir), deviceorientation(deviceorientation) {

	}

	ClientActionPacket(): stickdir(0), deviceorientation(0) {

	}
	bool readFromStream(Stream& st);
	bool writeToStream(Stream& st);
	uint8 stickdir;
	uint8 deviceorientation;
};


#endif