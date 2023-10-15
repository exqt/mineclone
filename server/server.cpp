#include "server.hpp"

#include <iostream>
#include <thread>
#include <algorithm>

Server::Server(int port) {
  if (enet_initialize () != 0) {
    std::cerr << "An error occurred while initializing ENet.\n" << std::endl;
    std::exit(1);
  }

  ENetAddress address = {0};

  address.host = ENET_HOST_ANY;
  address.port = 7878;

  #define MAX_CLIENTS 32

  host = enet_host_create(&address, MAX_CLIENTS, 2, 0, 0);
  if (host == NULL) {
    std::cerr << "An error occurred while trying to create an ENet server host.\n" << std::endl;
    std::exit(1);
  }

  game = new Game();

  // register RPCs
  rpcs["setBlock"] = std::bind(&Game::setBlock, game, std::placeholders::_1);
  rpcs["getChunkData"] = std::bind(&Game::getChunkData, game, std::placeholders::_1);
}

Server::~Server() {

}

void Server::onConnect(ENetEvent& event) {
  std::cout << "A new client connected from " << event.peer->address.host << ":" << event.peer->address.port << "." << std::endl;

  User* user = new User();
  user->peer = event.peer;
  user->name = "Test";
  users.push_back(user);

  event.peer->data = user;
}

void Server::onReceive(ENetEvent& event) {
  // std::cout << "A packet of length " << event.packet->dataLength << " containing " << event.packet->data << " was received from " << event.peer->data << " on channel " << event.channelID << "." << std::endl;

  User* user = (User*)event.peer->data;
  DataReadStream stream;
  stream.data = std::vector<std::byte>((std::byte*)event.packet->data, (std::byte*)event.packet->data + event.packet->dataLength);

  auto type = stream.pop<GamePacketDataType>();
  if (type == GamePacketDataType::RPC_CALL) {
    std::string methodName = stream.popString();
    if (rpcs.find(methodName) != rpcs.end()) {
      auto f = rpcs[methodName];
      auto data = f(stream).data;

      DataWriteStream retStream;
      retStream.push<GamePacketDataType>(GamePacketDataType::RPC_RESPONSE);
      retStream.pushString(methodName);
      retStream.data.insert(retStream.data.end(), data.begin(), data.end());

      send(user, retStream.data);
    }
    else {
      std::cout << "ERROR: unknown method " << methodName << std::endl;
    }
  }
  else if (type == GamePacketDataType::GAME_OBJECT_CREATE) {
    auto id = stream.pop<ObjectId>();
  }


  /* Clean up the packet now that we're done using it. */
  enet_packet_destroy(event.packet);
}

void Server::onDisconnect(ENetEvent& event) {
  std::cout << event.peer->data << " disconnected." << std::endl;
  event.peer->data = nullptr;

  std::remove_if(users.begin(), users.end(), [&](User* user) {
    return user->peer == event.peer;
  });
}

void Server::service() {
  std::cout << "listening for connections on port " << host->address.port << std::endl;

  ENetEvent event;
  int result;
  const int GAME_TICKS_PER_SECOND = 60;

  while (true) {
    while ((result = enet_host_service(host, &event, 1)) > 0) {
      switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
          onConnect(event);
          break;
        case ENET_EVENT_TYPE_RECEIVE:
          onReceive(event);
          break;
        case ENET_EVENT_TYPE_DISCONNECT:
          onDisconnect(event);
          break;
        case ENET_EVENT_TYPE_NONE:
          break;
      }
    }

    // update?

    std::this_thread::sleep_for(std::chrono::milliseconds(1000/GAME_TICKS_PER_SECOND));
  }

  enet_host_destroy(host);
  enet_deinitialize();
}

void Server::send(User* user, std::vector<std::byte> data) {
  ENetPacket* enetPacket = enet_packet_create(&data[0], data.size(), ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(user->peer, 0, enetPacket);
}

void Server::broadcast(std::vector<std::byte> data) {
  ENetPacket* enetPacket = enet_packet_create(&data[0], data.size(), ENET_PACKET_FLAG_RELIABLE);
  enet_host_broadcast(host, 0, enetPacket);
}
