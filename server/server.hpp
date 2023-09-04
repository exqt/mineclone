#pragma once

extern "C" {
  #include <enet/enet.h>
}

#include "user.hpp"
#include <vector>

class Server {
public:
  Server();
  ~Server();

  void service();

private:
  ENetHost* host;
  std::vector<User*> users;

  void onConnect(ENetEvent& event);
  void onReceive(ENetEvent& event);
  void onDisconnect(ENetEvent& event);
};
