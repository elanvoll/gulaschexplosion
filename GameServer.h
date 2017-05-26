#ifndef _INC_GAMESERVER_H

#define _INC_GAMESERVER_H
class GameServer {
public:
  GameServer(int port)
    : server(port){}
  ~GameServer();
  void begin();
  void registerGet(String url, Page<GetHandler> page);
  void registerPost(String url, Page<PostHandler> page);
  void doWork();
private:
  WiFiServer server;
  std::list<WiFiClient> currentClients;
  GetPageMap getHandlers;
  PostPageMap postHandlers;
};
#endif