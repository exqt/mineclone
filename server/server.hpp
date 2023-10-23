#pragma once

extern "C" {
  #include <enet/enet.h>
}

#include <vector>
#include <map>
#include <functional>
#include <optional>

#include "../common/byteStream.hpp"
#include "../common/networkObject.hpp"

struct User {
  int id;
  std::string name;
  ENetPeer* peer;
  NetworkObjectId NetworkObjectId;
};

using RPC = std::function<void(User*, DataReadStream&)>;

class Server {
public:
  Server(int port = 7878);
  ~Server();

  void service(RPC fn);

  void send(User* user, std::vector<std::byte> data);
  void broadcast(std::vector<std::byte> data);

  std::function<void(User*)> onConnectCallback, onDisconnectCallback;

private:
  ENetHost* host;
  int nextUserId = 1;
  std::vector<User*> users;

  void onConnect(ENetEvent& event);
  void onReceive(ENetEvent& event, RPC& fn);
  void onDisconnect(ENetEvent& event);
};
