#include <list>

#include "defines.h"
#include "GameRoundGenerator.h"
#include "GameRound.h"
#include "Packets.h"


#ifdef DEBUG_BUILD
#define TIMEOUTSECONDS 50
#else
#define TIMEOUTSECONDS 10
#endif

GameColor allGameColors[] = { {"green", 0x008000}, {"dark", 0x000000},
    {"red", 0x800000}, {"blue", 0x000080}, {"white", 0x808080}, {"yellow", 0x808000},
	{"pink", 0x800040}};

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


GameRound* GameRoundGenerator::genTrivialSwitchRound() {
	Serial.println("genTrivialSwitchRound");
	// this has to be clicked
	GameColor correctColor = getRandomGameColor();
	// this is written, error
	GameColor mentionedColor = getRandomGameColorExcept(&correctColor, 1);

	GameStickDir correctStickDir = getRandomStickDir();
	uint8 correctActingPlayerId = rand() % PLAYERS;
	uint8 correctMessagePlayerId = rand() % PLAYERS;
	uint8 playerSwitchedMsgPlayerId = rand() % PLAYERS;
	if (playerSwitchedMsgPlayerId == correctMessagePlayerId) {
		playerSwitchedMsgPlayerId++;
		playerSwitchedMsgPlayerId %= PLAYERS;
	}

	const int reserverdColorNum = 3;

	GameColor reserverdColor[reserverdColorNum];
	reserverdColor[0] = correctColor;
	for(int i=1; i<reserverdColorNum; ++i) {
		reserverdColor[i] = getRandomGameColor();
	}

	std::list<ServerGameStartPacket> instructions;
	for(int playerId=0; playerId<PLAYERS; ++playerId) {
		char buf[80];
		const char* format = "Press %s if available.";
		if(correctMessagePlayerId == playerId) {
			sprintf(buf, format, mentionedColor.name);
		} else if(playerSwitchedMsgPlayerId == playerId) {
			sprintf(buf, "Player %i means %s instead of %s.", correctMessagePlayerId, correctColor.name, mentionedColor.name);
		} else {
			sprintf(buf, format, reserverdColor[rand()%reserverdColorNum].name);
		}
		ServerGameStartPacket playerPacket;
		playerPacket.timeoutseconds = TIMEOUTSECONDS;
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

// REQUIRES 3 Players
GameRound* GameRoundGenerator::genLyingSwitchRound() {
	Serial.println("genLyingSwitchRound");
	if(PLAYERS < 3) {
		Serial.println("CANT GEN LYING SWITCH WITH < 3 PLAYERS!");
		return genTrivialSwitchRound();
	}
	// this has to be clicked
	GameColor correctColor = getRandomGameColor();

	GameStickDir correctStickDir = getRandomStickDir();
	uint8 correctActingPlayerId = rand() % PLAYERS;
	uint8 correctMessagePlayerId = rand() % PLAYERS;
	uint8 playerSwitchedMsgPlayerId = rand() % PLAYERS;
	if (playerSwitchedMsgPlayerId == correctMessagePlayerId) {
		playerSwitchedMsgPlayerId++;
		playerSwitchedMsgPlayerId %= PLAYERS;
	}
	uint8 youLiedPlayerId = rand() % PLAYERS;
	while (youLiedPlayerId == correctMessagePlayerId || youLiedPlayerId == playerSwitchedMsgPlayerId) {
		youLiedPlayerId++;
		youLiedPlayerId %= PLAYERS;
	}
	Serial.printf("players: cormess %i, switch %i, lie! %i\n", correctMessagePlayerId, playerSwitchedMsgPlayerId, youLiedPlayerId);

	const int reserverdColorNum = 3;

	GameColor reserverdColor[reserverdColorNum];
	reserverdColor[0] = correctColor;
	for(int i=1; i<reserverdColorNum; ++i) {
		reserverdColor[i] = getRandomGameColor();
	}

	std::list<ServerGameStartPacket> instructions;
	for(int playerId=0; playerId<PLAYERS; ++playerId) {
		char buf[80];
		const char* format = "Press %s if available.";
		if(correctMessagePlayerId == playerId) {
			sprintf(buf, format, correctColor.name);
		} else if(playerSwitchedMsgPlayerId == playerId) {
			sprintf(buf, "Player %i means %s instead of %s.",correctMessagePlayerId, getRandomGameColor().name, correctColor.name);
		} else if(youLiedPlayerId == playerId) {
			sprintf(buf, "Player %i is not telling the truth.", playerSwitchedMsgPlayerId);
		} else {
			sprintf(buf, format, reserverdColor[rand()%reserverdColorNum].name);
		}
		ServerGameStartPacket playerPacket;
		playerPacket.timeoutseconds = TIMEOUTSECONDS;
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


GameRound* GameRoundGenerator::genTrivialMisleadingSwitchRound() {
	Serial.println("genTrivialMisleadingSwitchRound");
	GameColor correctColor = getRandomGameColor();
	GameStickDir correctStickDir = getRandomStickDir();
	uint8 correctActingPlayerId = rand() % PLAYERS;
	uint8 correctMessagePlayerId = rand() % PLAYERS;

	uint8 misleadingSwitchPlayerId = rand() % PLAYERS;
	if(misleadingSwitchPlayerId == correctMessagePlayerId) {
		++misleadingSwitchPlayerId;
		misleadingSwitchPlayerId %= PLAYERS;
	}

	const int reserverdColorNum = 3;

	GameColor reserverdColor[reserverdColorNum];
	reserverdColor[0] = correctColor;
	for(int i=1; i<reserverdColorNum; ++i) {
		reserverdColor[i] = getRandomGameColor();
	}

	std::list<ServerGameStartPacket> instructions;
	for(int playerId=0; playerId<PLAYERS; ++playerId) {
		char buf[80];
		const char* format = "Press %s if available.";
		if(correctMessagePlayerId == playerId) {
			sprintf(buf, format, correctColor.name);
		} else if(misleadingSwitchPlayerId == playerId){
			// this will always be misleading
			sprintf(buf, "Player %i means %s instead of %s.",
				rand()%PLAYERS, reserverdColor[(rand()%(reserverdColorNum-1))+1].name,
				reserverdColor[(rand()%(reserverdColorNum-1))+1].name);
		}else{
			sprintf(buf, format, reserverdColor[rand()%reserverdColorNum].name);
		}
		ServerGameStartPacket playerPacket;
		playerPacket.timeoutseconds = TIMEOUTSECONDS;
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


GameRound* GameRoundGenerator::genTrivialRound() {
	Serial.println("genTrivialRound");
	GameColor correctColor = getRandomGameColor();
	GameStickDir correctStickDir = getRandomStickDir();
	uint8 correctActingPlayerId = rand() % PLAYERS;
	uint8 correctMessagePlayerId = rand() % PLAYERS;

	const int reserverdColorNum = 3;

	GameColor reserverdColor[reserverdColorNum];
	reserverdColor[0] = correctColor;
	for(int i=1; i<reserverdColorNum; ++i) {
		reserverdColor[i] = getRandomGameColor();
	}

	std::list<ServerGameStartPacket> instructions;
	for(int playerId=0; playerId<PLAYERS; ++playerId) {
		char buf[80];
		const char* format = "Press %s if available.";
		if(correctMessagePlayerId == playerId) {
			sprintf(buf, format, correctColor.name);
		} else {
			sprintf(buf, format, reserverdColor[rand()%reserverdColorNum].name);
		}
		ServerGameStartPacket playerPacket;
		playerPacket.timeoutseconds = TIMEOUTSECONDS;
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

void setTimeout(GameRound *r, uint32 timeoutseconds) {
	for(std::list<ServerGameStartPacket>::iterator itr = r->instructions.begin(); itr != r->instructions.end(); ++itr) {
		itr->timeoutseconds = timeoutseconds;
	}
}

void strrevMutation(GameRound *r) {
	Serial.println("strrevMutation");
	if(r == NULL) {
		Serial.println("Cant mutate empty round?!");
		return;
	}
	for(std::list<ServerGameStartPacket>::iterator itr = r->instructions.begin(); itr != r->instructions.end(); ++itr) {
		if(rand()%2 == 0) {
			String tmp;
			for(int i = itr->text.length()-1; i >= 0; --i) {
				tmp += itr->text[i];
			}
			itr->text = tmp;
		}
	}
}

GameRound* GameRoundGenerator::newRound(int gameRound) {
	GameRound *r = NULL;
#ifdef DEBUG_BUILD

	switch((gameRound-1) % 4) {
		case 0:
		r = genTrivialRound();
		break;
		case 1:
		r = genTrivialSwitchRound();
		break;
		case 2:
		r = genTrivialMisleadingSwitchRound();
		break;
		case 3:
		r = genLyingSwitchRound();
		break;
	}
	switch((gameRound-1) % 8) {
		case 4:
		case 5:
		case 6:
		case 7:
		strrevMutation(r);
	}
#else
	if (gameRound <= 3) {
		r = genTrivialRound();
		setTimeout(r, 30 - gameRound*5);
	}
	else if(gameRound <= 7) {
		if(rand()%2) {
			r = genTrivialRound();
			setTimeout(r, 10);
		} else {
			r = genTrivialSwitchRound();
			setTimeout(r, 20);
		}
	}
	else if(gameRound <= 12) {
		if(rand()%2) {
			r = genTrivialRound();
		} else {
			r = genTrivialSwitchRound();
		}
		if((rand() % 3) == 0) {
			strrevMutation(r);
			setTimeout(r, 20);
		}
	}
	else if(gameRound <= 16) {
		switch(rand()%3) {
			case 0:
			r = genTrivialRound();
			break;
			case 1:
			r = genTrivialSwitchRound();
			break;
			case 2:
			r = genTrivialMisleadingSwitchRound();
			setTimeout(r, 20);
			break;
		}
		if((rand() % 3) == 0)
			strrevMutation(r);
	}
	else {
		switch(rand()%4) {
			case 0:
			r = genTrivialRound();
			break;
			case 1:
			r = genTrivialSwitchRound();
			break;
			case 2:
			r = genTrivialMisleadingSwitchRound();
			break;
			case 3:
			r = genLyingSwitchRound();
			break;
		}
		if((rand() % 3) == 0)
			strrevMutation(r);
	}

	if (gameRound > 20) {
		int timeout = 10 - (gameRound -20)/2;
		if(timeout < 3) {
			timeout = 3;
		}
		setTimeout(r, timeout);
	}
#endif

	if(!r) {
		Serial.println("Round generation failed, will die");
	}
	for(auto itr = r->instructions.begin(); itr != r->instructions.end(); ++itr) {
		itr->gameround = gameRound;
	}
	return r;
}