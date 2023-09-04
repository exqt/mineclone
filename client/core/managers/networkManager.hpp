#pragma once

#include <enet/enet.h>
#undef far // fuck microsoft
#include <vector>
#include <string>

class NetworkManager {
public:
  static NetworkManager& Instance() {
    static NetworkManager instance;
    return instance;
  }

  NetworkManager();
  ~NetworkManager();

  bool connect(std::string hostName, int port);
  void disconnect();
  void service();

  bool send(std::string str);

private:
  ENetHost* host;
  ENetPeer* peer;
  bool isConnected;
};
