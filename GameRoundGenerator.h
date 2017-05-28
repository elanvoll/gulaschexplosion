#pragma once

#include <stdint.h>

typedef uint32_t uint32;


class GameRound;

struct GameColor {
	const char* name;
	uint32 value;
};

struct GameStickDir {
	const char* name;
	int stickdir;
};

struct GameRoundGenerator
{
	static GameRound* newRound(int gameRound);
	static GameRound* genTrivialRound();
	static GameRound* genTrivialSwitchRound();
	static GameRound* genTrivialMisleadingSwitchRound();
	static GameRound* genLyingSwitchRound();
	// helpers
	static GameColor getRandomGameColor();
	static GameColor getRandomGameColorExcept(GameColor* retricted, int len);
	static GameStickDir getRandomStickDir();
};