#pragma once

#include <string>
#include "server.hpp"
#include "../common/byteStream.hpp"
#include "../common/networkObject.hpp"
#include "../common/world.hpp"
#include "../common/block.hpp"
#include "../common/networkObject.hpp"
#include "terrain.hpp"

class Game {
public:
  Game(Server* server);

  void update();
  void onRPC(User* user, DataReadStream& data);

  void onConnect(User* user);
  void onDisconnect(User* user);

private:
  Server* server;

  World* world;
  Terrain* terrain;

  NetworkObjectId nextNetworkObjectId = 1; // 0 is reserved for the server
  std::map<NetworkObjectId, NetworkObjectData> networkObjects, ownedNetworkObjects;

  std::map<NetworkObjectId, NetworkObjectData> playerNetworkObjects;
  std::map<std::tuple<int,int,int>, NetworkObjectData> chunkNetworkObjects;

  std::map<std::string, std::function<void(User*, DataReadStream&)>> rpcs;

  void createNetworkObject(std::string type, NetworkObjectOwner owner, std::vector<std::byte> data);
  void syncNetworkObject(NetworkObjectData data, User* user = nullptr);
  void destroyNetworkObject(NetworkObjectId id);
  void placeBlock(int x, int y, int z, BlockType block);

  void registerRPCs();
};
