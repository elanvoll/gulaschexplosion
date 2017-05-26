#ifndef _INC_GAMESERVER_H
#define _INC_GAMESERVER_H

#include <WiFiClient.h>
#include <WiFiServer.h>

#include <list>

#include "GameServerProxy.h"

// actual gameserver. runs on host only

class GameServer : public AbstractGameServer {
public:
  GameServer(int port)
    : server(port){}
  ~GameServer();
  void begin();
  void doWork();
private:
  WiFiServer server;
  std::list<WiFiClient> currentClients;
};
#endif