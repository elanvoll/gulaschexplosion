#ifndef _INC_GAMESERVER_H

#include <WiFiClient.h>
#include <WiFiServer.h>

#include <list>


#define _INC_GAMESERVER_H
class GameServer {
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