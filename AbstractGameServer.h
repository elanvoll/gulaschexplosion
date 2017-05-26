#pragma once

#include "GameUI.h"

class AbstractGameServer
{
public:
	AbstractGameServer(GameUI* ui) : ui(ui) {

	}
	virtual ~AbstractGameServer(){

	}
	virtual void doWork() = 0;

	GameUI* ui;
};