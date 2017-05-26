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
 ServerGameStartPacket(round, text = "player 3 shall press left", led0 = 0xffffffff, led1, led2, led3, timeoutseconds - delta)
 ClientActionPacket(stickdir = 2, device orientation)
 ServerClientActionLogPacket(playerid, stickdir, deviceorientation)
  Unterscheidung:
   SeverGameOver(timeout true/false, correctplayerid, correctstickdir, correctdeviceor)
  Oder
   SeverGameSuccess!()
  Host klickt auf next round:
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
	
	bool readFromStream(Stream& st);
	bool writeToStream(Stream& st);
	uint8 playerId;
};

struct ServerGameStartPacket : GamePacket
{
	bool readFromStream(Stream& st);
	bool writeToStream(Stream& st);
	
	int round;
	String text;
	int led0;
	int led1;
	int led2;
	int led3;
	int timeoutseconds;
};

struct ServerClientActionLogPacket : GamePacket
{
	bool readFromStream(Stream& st);
	bool writeToStream(Stream& st);
	int stickdir;
	int deviceorientation;
	uint8 playerid;
};

struct ClientActionPacket : GamePacket {
	bool readFromStream(Stream& st);
	bool writeToStream(Stream& st);
	uint8 stickdir;
	uint8 deviceorientation;
};


#endif