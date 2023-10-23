#pragma once

extern "C" {
  #include <enet/enet.h>
}

#include <vector>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "../common/byteStream.hpp"
#include "../common/networkObject.hpp"

using RPCHandler = std::function<void(DataReadStream&)>;

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
  void service(RPCHandler fn);

  void callRPC(std::string name, std::vector<std::byte>& data);
  void send(std::vector<std::byte>& data);

private:
  ENetHost* host;
  ENetPeer* peer;
  bool isConnected;

  void onConnect(ENetEvent& event);
  void onReceive(ENetEvent& event, RPCHandler& fn);
  void onDisconnect(ENetEvent& event);
};
