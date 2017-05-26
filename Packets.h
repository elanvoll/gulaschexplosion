#ifndef _INC_PACKET_H

#define _INC_PACKET_H


#include <Stream.h>


class GamePacket
{
public:
	GamePacket();
	~GamePacket();
};



/*

Flow example:
 ClientJoinPacket
 ServerJoinAckPacket(playerid = 2 [Range: 1..3?5?])
 ServerGameStartPacket(text = "player 3 shall press left", led0 = 0xffffffff, led1, led2, led3, )
 ClientActionPacket(stickdir = 2, device orientation)
 ServerClientActionLogPacket(playerid, stickdir, deviceorientation)
  Unterscheidung:
   SeverGameOver(correctplayerid, correctstickdir, correctdeviceor)
  Oder
   SeverGameSuccess!()
  Host klickt auf next round:
   ServerGameStartPacket wieder
*/

class ClientJoinPacket : GamePacket
{
public:
	ClientJoinPacket();
	~ClientJoinPacket();
	
};


class ServerJoinAckPacket : GamePacket
{
public:
	ServerJoinAckPacket();
	~ServerJoinAckPacket();
	
	bool readFromStream(Stream& st);
	uint8 playerId;
};


#endif