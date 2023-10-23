#include "server.hpp"

#include <iostream>
#include <thread>
#include <algorithm>
#include "../common/networkObject.hpp"

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
}

Server::~Server() {
  for (auto user : users) {
    delete user;
  }

  enet_host_destroy(host);
  enet_deinitialize();
}

void Server::service(RPC fn) {
  ENetEvent event;
  int result;

  while ((result = enet_host_service(host, &event, 1)) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        onConnect(event);
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        onReceive(event, fn);
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        onDisconnect(event);
        break;
      case ENET_EVENT_TYPE_NONE:
        break;
    }
  }
}

void Server::send(User* user, std::vector<std::byte> data) {
  ENetPacket* enetPacket = enet_packet_create(&data[0], data.size(), ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(user->peer, 0, enetPacket);
}

void Server::broadcast(std::vector<std::byte> data) {
  ENetPacket* enetPacket = enet_packet_create(&data[0], data.size(), ENET_PACKET_FLAG_RELIABLE);
  enet_host_broadcast(host, 0, enetPacket);
}

void Server::onConnect(ENetEvent& event) {
  std::cout << "A new client connected from " << event.peer->address.host << ":" << event.peer->address.port << "." << std::endl;

  User* user = new User();
  user->peer = event.peer;
  user->name = "Test";
  user->id = nextUserId++;
  users.push_back(user);

  event.peer->data = user;

  if (onConnectCallback) {
    onConnectCallback(user);
  }
}

void Server::onReceive(ENetEvent& event, RPC& fn) {
  // std::cout << "A packet of length " << event.packet->dataLength << " containing " << event.packet->data << " was received from " << event.peer->data << " on channel " << event.channelID << "." << std::endl;

  User* user = (User*)event.peer->data;
  DataReadStream stream;
  stream.data = std::vector<std::byte>((std::byte*)event.packet->data, (std::byte*)event.packet->data + event.packet->dataLength);

  fn(user, stream);

  /* Clean up the packet now that we're done using it. */
  enet_packet_destroy(event.packet);
}

void Server::onDisconnect(ENetEvent& event) {
  User* user = (User*)event.peer->data;

  std::cout << event.peer->data << " disconnected." << std::endl;

  std::remove_if(users.begin(), users.end(), [&](User* user) {
    return user->peer == event.peer;
  });

  if (onDisconnectCallback) {
    onDisconnectCallback(user);
  }

  delete user;
  event.peer->data = nullptr;
}
