#ifndef _INC_GAMESERVER_H
#define _INC_GAMESERVER_H

#include <WiFiClient.h>
#include <WiFiServer.h>

#include <list>

#include "AbstractGameServer.h"
#include "GameRound.h"


enum gameserverstate_t
{
  GAME_SERVER_STATE_LISTEN,   // waiting for players
  GAME_SERVER_STATE_READY,    // awaiting start from host
  GAME_SERVER_STATE_RUNNING  // awaiting user actions, time runs
};

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

  uint8 serverState = GAME_SERVER_STATE_LISTEN;
};
#endif