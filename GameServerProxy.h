#pragma once

#include "AbstractGameServer.h"


// Sends stuff

class GameServerProxy : public AbstractGameServer
{
public:
	GameServerProxy(IPAddress ip, int port) : ip(ip), port(port) {}
	~GameServerProxy();

	bool begin();
	
	IPAddress ip;
	int port;
	WiFiClient client;
};