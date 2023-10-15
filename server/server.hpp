#pragma once

extern "C" {
  #include <enet/enet.h>
}

#include <vector>
#include <map>
#include <functional>
#include <optional>

#include "../common/byteStream.hpp"
#include "../common/gamePacket.hpp"

#include "game.hpp"

struct Object {
  ObjectId id;
  std::vector<std::byte> data;
};

struct User {
  std::string name;
  ENetPeer* peer;
};

using RPC = std::function<DataWriteStream(DataReadStream&)>;

class Server {
public:
  Server(int port = 7878);
  ~Server();

  void service();

private:
  Game* game;

  ENetHost* host;
  std::vector<User*> users;
  std::map<unsigned long long, Object> objects;
  std::map<std::string, RPC> rpcs;

  void onConnect(ENetEvent& event);
  void onReceive(ENetEvent& event);
  void onDisconnect(ENetEvent& event);

  void send(User* user, std::vector<std::byte> data);
  void broadcast(std::vector<std::byte> data);
};
