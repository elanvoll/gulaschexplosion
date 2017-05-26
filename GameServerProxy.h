#pragma once

#include "AbstractGameServer.h"


// Sends stuff

class GameServerProxy : public AbstractGameServer
{
public:
	GameServerProxy(IPAddress ip, int port) : ip(ip), port(port) {}
	~GameServerProxy();
	
	IPAddress ip;
	int port;
};