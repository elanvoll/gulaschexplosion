#include <list>

#include "defines.h"
#include "GameRoundGenerator.h"
#include "GameRound.h"
#include "Packets.h"


GameColor allGameColors[] = { {"green", 0x008000}, {"dark", 0x000000},
    {"red", 0x800000}, {"blue", 0x000080}, {"white", 0x808080}, {"yellow", 0x808000},
	{"purple", 0x800080}};

GameStickDir allStickDirs[] = { {"up", STICK_DIR_UP}, {"down", STICK_DIR_DOWN}, {"left", STICK_DIR_LEFT}, {"right", STICK_DIR_RIGHT}};

GameStickDir GameRoundGenerator::getRandomStickDir() {
	return allStickDirs[rand() % (sizeof(allStickDirs)/sizeof(GameStickDir))];
}

GameColor GameRoundGenerator::getRandomGameColor() {
	return allGameColors[rand() % (sizeof(allGameColors)/sizeof(GameColor))];
}

GameColor GameRoundGenerator::getRandomGameColorExcept(GameColor* restricted, int len) {
	GameColor c;
	bool dup;
	do {
		c = getRandomGameColor();
		dup = false;
		for(int i=0; i<len; ++i) {
			if (c.value == restricted[i].value) {
				dup=true;
				break;
			}
		}
	} while (dup);
	return c;
}

int ledToDir(int led) {
	switch(led) {
		case 1:
		return STICK_DIR_UP;
		case 2:
		return STICK_DIR_LEFT;
		case 3:
		return STICK_DIR_RIGHT;
		case 4:
		return STICK_DIR_DOWN;
		default:
		Serial.printf("FATAL! unk led %d\n", led);
	}
}

void setLedColor(int led, GameColor* color, ServerGameStartPacket* packet) {
	uint32* addr;
	switch(led) {
		case 1:
		addr = &packet->led1;
		break;
		case 2:
		addr = &packet->led2;
		break;
		case 3:
		addr = &packet->led3;
		break;
		case 4:
		addr = &packet->led4;
		break;
	}
	*addr = color->value;
}

GameRound* GameRoundGenerator::genTrivialRound() {
	GameColor correctColor = getRandomGameColor();
	GameStickDir correctStickDir = getRandomStickDir();
	uint8 correctActingPlayerId = rand() % PLAYERS;
	uint8 correctMessagePlayerId = rand() % PLAYERS;

	const int reserverdColorNum = 3;
	const int timeoutseconds = 10;

	GameColor reserverdColor[reserverdColorNum];
	reserverdColor[0] = correctColor;
	for(int i=1; i<reserverdColorNum; ++i) {
		reserverdColor[i] = getRandomGameColor();
	}

	std::list<ServerGameStartPacket> instructions;
	for(int playerId=0; playerId<PLAYERS; ++playerId) {
		char buf[40];
		const char* format = "Press %s if available.";
		if(correctMessagePlayerId == playerId) {
			sprintf(buf, format, correctColor.name);
		} else {
			sprintf(buf, format, reserverdColor[rand()%reserverdColorNum].name);
		}
		ServerGameStartPacket playerPacket;
		playerPacket.timeoutseconds = timeoutseconds;
		playerPacket.text = String(buf);
		for(int led = 1; led <= 4; ++led) {
			if (playerId == correctActingPlayerId &&
				ledToDir(led) == correctStickDir.stickdir) {
				setLedColor(led, &correctColor, &playerPacket);
			} else {
				GameColor faultycolor = getRandomGameColorExcept(reserverdColor, reserverdColorNum);
				setLedColor(led, &faultycolor, &playerPacket);
			}
		}
		instructions.push_back(playerPacket);
	}

	std::list<ServerClientActionLogPacket> correctSeq;
	correctSeq.push_back(ServerClientActionLogPacket(correctStickDir.stickdir, 0, correctActingPlayerId));
	return new GameRound(instructions, correctSeq);
}

GameRound* GameRoundGenerator::newRound(int gameRound) {
	GameRound *r = genTrivialRound();


	for(auto itr = r->instructions.begin(); itr != r->instructions.end(); ++itr) {
		itr->gameround = gameRound;
	}
	return r;
}