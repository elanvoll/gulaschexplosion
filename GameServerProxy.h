#pragma once

#include "AbstractGameServer.h"


// Sends stuff

class GameServerProxy : public AbstractGameServer
{
public:
	GameServerProxy(IPAddress ip, int port, GameUI* ui) : AbstractGameServer(ui), ip(ip), port(port) {}
	~GameServerProxy();

	bool begin();
	virtual void doWork();

	IPAddress ip;
	int port;
	WiFiClient client;
};