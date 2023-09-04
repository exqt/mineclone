#include "server.hpp"

#include <iostream>

Server::Server() {
  if (enet_initialize () != 0) {
    std::cout << "An error occurred while initializing ENet.\n" << std::endl;
    std::exit(1);
  }

  ENetAddress address = {0};

  address.host = ENET_HOST_ANY;
  address.port = 7878;

  #define MAX_CLIENTS 32

  /* create a server */
  host = enet_host_create(&address, MAX_CLIENTS, 2, 0, 0);
  if (host == NULL) {
    std::cerr << "An error occurred while trying to create an ENet server host.\n" << std::endl;
    std::exit(1);
  }
}

Server::~Server() {

}

void Server::onConnect(ENetEvent& event) {
  std::cout << "A new client connected from " << event.peer->address.host << ":" << event.peer->address.port << "." << std::endl;

  User* user = new User();
  user->id = event.peer->incomingPeerID;
  user->name = "Test";
  users.push_back(user);

  event.peer->data = user;
}

void Server::onReceive(ENetEvent& event) {
  std::cout << "A packet of length " << event.packet->dataLength << " containing " << event.packet->data << " was received from " << event.peer->data << " on channel " << event.channelID << "." << std::endl;
  /* Clean up the packet now that we're done using it. */
  enet_packet_destroy(event.packet);
}

void Server::onDisconnect(ENetEvent& event) {
  std::cout << event.peer->data << " disconnected." << std::endl;
  /* Reset the peer's client information. */
  event.peer->data = nullptr;
}

void Server::service() {
  std::cout << "listening for connections on port " << host->address.port << std::endl;

  ENetEvent event;
  int result;
  while (true) {
    /* Wait up to 1000 milliseconds for an event. */
    while ((result = enet_host_service(host, &event, 1000)) > 0) {
      std::cout << "Event type: " << event.type << std::endl;
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
  }

  enet_host_destroy(host);
  enet_deinitialize();
}
