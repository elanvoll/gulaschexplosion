#ifndef _INC_GAMESERVER_H
#define _INC_GAMESERVER_H

#include <WiFiClient.h>
#include <WiFiServer.h>

#include <list>

#include "AbstractGameServer.h"
#include "GameRound.h"
// actual gameserver. runs on host only

class GameServer : public AbstractGameServer {
public:
  GameServer(int port, GameUI* ui)
    : AbstractGameServer(ui), server(port){}
  virtual ~GameServer();
  void begin();
  void doWork();
private:
  void broadcast(BasePacket* p);
  GameRound* generateGameRound();
  void handleClientInteraction(ClientActionPacket& p, uint8 userid);
  void startGame();
  void userActionHost(uint8 direction);
  WiFiServer server;
  std::list<WiFiClient> currentClients;
  std::list<ServerClientActionLogPacket> correctSequence;
  int currentRound = 1;
  uint32 timeoutms = 0;
};
#endif