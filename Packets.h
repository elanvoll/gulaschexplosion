#ifndef _INC_PACKET_H

#define _INC_PACKET_H



class GamePacket
{
public:
	GamePacket();
	~GamePacket();
	
};

class ClientJoinPacket : GamePacket
{
public:
	ClientJoinPacket();
	~ClientJoinPacket();
	
};

#endif