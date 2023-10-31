#include "game.hpp"

#include <iostream>

namespace {
  int idiv(int x, int y) {
    return x >= 0 ? x/y : -((-x - 1) / y + 1);
  }
}

Game::Game(Server* server) : server(server) {
  world = new World();
  terrain = new Terrain(1337);

  registerRPCs();
}

void Game::update() {
  const int radius = 128;

  std::map<std::string, std::vector<NetworkObjectData>> objectsByType;
  for (auto& [id, object] : networkObjects) {
    objectsByType[object.type].push_back(object);
  }

  for (auto player : objectsByType["PLAYER"]) {
    auto data = player.data;
    DataReadStream stream(data);
    auto x = stream.pop<float>();
    auto y = stream.pop<float>();
    auto z = stream.pop<float>();

    int px = idiv(x, ChunkData::BLOCKS_X);
    int pz = idiv(z, ChunkData::BLOCKS_Z);
    int cx = idiv(x - radius, ChunkData::BLOCKS_X);
    int cz = idiv(z - radius, ChunkData::BLOCKS_Z);
    int dx = idiv(x + radius, ChunkData::BLOCKS_X);
    int dz = idiv(z + radius, ChunkData::BLOCKS_Z);

    std::vector<std::tuple<int,int>> toLoad;
    for (int ix = cx; ix <= dx; ix++) {
      for (int iz = cz; iz <= dz; iz++) {
        toLoad.push_back({ix, iz});
      }
    }

    std::sort(toLoad.begin(), toLoad.end(), [&](auto a, auto b) {
      auto [ax, az] = a;
      auto [bx, bz] = b;
      return std::abs(px - ax) + std::abs(pz - az) < std::abs(px - bx) + std::abs(pz - bz);
    });

    for (auto [ix, iz] : toLoad) {
      if (!world->isLoadedChunk(ix, 0, iz)) {
        auto chunkData = terrain->getChunkData(ix, 0, iz);
        world->setChunkData(ix, 0, iz, chunkData);

        auto stream = DataWriteStream();
        stream.push<int>(ix);
        stream.push<int>(0);
        stream.push<int>(iz);
        stream.push<std::vector<std::byte>>(chunkData->toByteArray());

        createNetworkObject("CHUNK", 0, stream.data);
      }
    }
  }
}

void Game::onRPC(User* user, DataReadStream& stream) {
  std::string rpcName = stream.pop<std::string>();
  if (rpcs.find(rpcName) != rpcs.end()) {
    rpcs[rpcName](user, stream);
  } else {
    std::cout << "Unknown RPC: " << rpcName << std::endl;
  }
}

void Game::createNetworkObject(std::string type, NetworkObjectOwner owner, std::vector<std::byte> data) {
  NetworkObjectData object = {
    .id = nextNetworkObjectId++,
    .type = type,
    .owner = owner,
    .data = data
  };

  networkObjects[object.id] = object;
  if (type == "CHUNK") {
    auto stream = DataReadStream(data);
    auto x = stream.pop<int>();
    auto y = stream.pop<int>();
    auto z = stream.pop<int>();
    chunkNetworkObjectIds[{x, y, z}] = object.id;
  }

  DataWriteStream stream;
  stream.push<std::string>("OBJECT_SYNC");
  stream.push<std::vector<std::byte>>(object.toByteArray());

  server->broadcast(stream.data);
}

void Game::syncNetworkObject(NetworkObjectData data, User* user) {
  DataWriteStream stream;
  stream.push<std::string>("OBJECT_SYNC");
  stream.push<std::vector<std::byte>>(data.toByteArray());

  networkObjects[data.id].data = data.data;

  if (user) {
    server->send(user, stream.data);
  } else {
    server->broadcast(stream.data);
  }
}

void Game::destroyNetworkObject(NetworkObjectId id) {
  networkObjects.erase(id);

  DataWriteStream stream;
  stream.push<std::string>("OBJECT_DESTROY");
  stream.push<NetworkObjectId>(id);
  server->broadcast(stream.data);
}

void Game::placeBlock(int x, int y, int z, BlockType block) {
  int ox = idiv(x, ChunkData::BLOCKS_X);
  int oy = idiv(y, ChunkData::BLOCKS_Y);
  int oz = idiv(z, ChunkData::BLOCKS_Z);

  auto it = chunkNetworkObjectIds.find({ox, oy, oz});
  if (it == chunkNetworkObjectIds.end()) {
    std::cout << "ERROR: chunk not found" << std::endl;
    std::exit(1);
  }

  if (!world->isLoadedChunk(ox, oy, oz)) {
    std::cout << "ERROR: chunk not loaded" << std::endl;
    std::exit(1);
  }

  world->placeBlock(x, y, z, block);
  auto id = it->second;
  auto object = networkObjects[id];

  auto stream = DataWriteStream();
  stream.push<int>(ox);
  stream.push<int>(oy);
  stream.push<int>(oz);
  stream.push(world->getChunkData(ox, oy, oz)->toByteArray());
  object.data = stream.data;

  syncNetworkObject(object);
}

void Game::registerRPCs() {
  rpcs["OBJECT_CREATE"] = [&](User* user, DataReadStream& stream) {
    auto type = stream.pop<std::string>();
    auto data = stream.pop<std::vector<std::byte>>();
    auto owner = user->id;

    createNetworkObject(type, owner, data);
  };

  rpcs["OBJECT_SYNC"] = [&](User* user, DataReadStream& stream) {
    auto data = stream.pop<std::vector<std::byte>>();
    auto object = NetworkObjectData::fromByteArray(data);
    syncNetworkObject(object);
  };

  rpcs["OBJECT_DESTROY"] = [&](User* user, DataReadStream& stream) {
    auto id = stream.pop<NetworkObjectId>();
    destroyNetworkObject(id);
  };

  rpcs["PLACE_BLOCK"] = [&](User* user, DataReadStream& stream) {
    auto x = stream.pop<int>();
    auto y = stream.pop<int>();
    auto z = stream.pop<int>();
    auto block = stream.pop<BlockType>();

    placeBlock(x, y, z, block);
  };
}

void Game::onConnect(User* user) {
  std::cout << "User connected: " << user->id << std::endl;

  DataWriteStream stream;
  stream.push<std::string>("HELLO");
  stream.push<int>(user->id);
  server->send(user, stream.data);

  //
  auto playerDataStream = DataWriteStream();
  playerDataStream.push<float>(0);
  playerDataStream.push<float>(80.0);
  playerDataStream.push<float>(0);

  createNetworkObject("PLAYER", user->id, playerDataStream.data);

  // send all existing objects
  for (auto& [id, object] : networkObjects) {
    if (object.owner != user->id) {
      syncNetworkObject(object, user);
    }
  }
}

void Game::onDisconnect(User* user) {
  std::cout << "User disconnected: " << user->id << std::endl;

  for (auto& [id, object] : networkObjects) {
    if (object.owner == user->id) {
      destroyNetworkObject(id);
    }
  }
}
