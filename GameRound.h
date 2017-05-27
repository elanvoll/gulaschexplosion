#ifndef _INC_GAME_ROUND_H

#define _INC_GAME_ROUND_H

#include <list>

#include <stdint.h>


#include "Packets.h"

enum deviceorientation
{
	Undefined,
	Headup,
	Horizontal,
	headfirst
};

class GameRound
{
public:
	GameRound(std::list<ServerGameStartPacket> instructions, std::list<ServerClientActionLogPacket> correctSeq)
		: instructions(instructions), correctSeq(correctSeq) {

	}
	std::list<ServerGameStartPacket> instructions;

	std::list<ServerClientActionLogPacket> correctSeq;
};

#endif