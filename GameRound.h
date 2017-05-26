#ifndef _INC_GAME_ROUND_H

#define _INC_GAME_ROUND_H



enum deviceorientation
{
	Undefined,
	Headup,
	Horizontal,
	headfirst
};

class UserAction
{
public:
	UserAction();
	~UserAction();
	JoystickState joystick;

};

class GameRound
{
public:
	GameRound();
	~GameRound();
	

};

#endif