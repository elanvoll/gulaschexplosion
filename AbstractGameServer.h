#pragma once

class AbstractGameServer
{
public:
	virtual AbstractGameServer(Ui) : ui(ui) {

	}
	virtual ~AbstractGameServer() = 0;

	virtual void startGame() = 0;
	
	virtual void userAction() = 0;
	
	Ui ui;
};