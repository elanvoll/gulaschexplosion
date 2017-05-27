#include <list>

#include "defines.h"
#include "GameRoundGenerator.h"
#include "GameRound.h"
#include "Packets.h"


GameColor allGameColors[] = { {"green", 0x008000}, {"dark", 0x000000}, {"red", 0x800000}, {"blue", 0x000080}};

GameStickDir allStickDirs[] = { {"up", STICK_DIR_UP}, {"down", STICK_DIR_DOWN}, {"left", STICK_DIR_LEFT}, {"right", STICK_DIR_RIGHT}};

GameStickDir GameRoundGenerator::getRandomStickDir() {
	return allStickDirs[rand() % (sizeof(allStickDirs)/sizeof(GameStickDir))];
}

GameColor GameRoundGenerator::getRandomGameColor() {
	return allGameColors[rand() % (sizeof(allGameColors)/sizeof(GameColor))];
}

GameRound* GameRoundGenerator::genTrivialRound() {
	GameColor correctColor = getRandomGameColor();
}

GameRound* GameRoundGenerator::newRound(int gameRound) {

	std::list<ServerGameStartPacket> instructions;
	instructions.push_back(ServerGameStartPacket(1, "Player 2 must not press any button", 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 20));
	instructions.push_back(ServerGameStartPacket(1, "One player has to select green", 0xFF00FF00, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 20));
	instructions.push_back(ServerGameStartPacket(1, "If there is red, select it first", 0xFF00FF00, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 20));
	// richtig: player 1 geht zu led1 = oben
	std::list<ServerClientActionLogPacket> correctSeq;
	correctSeq.push_back(ServerClientActionLogPacket(STICK_DIR_UP, 0, 1));

	switch(gameRound) {
		case 1:
		default:
		return new GameRound(instructions, correctSeq);
	}
}