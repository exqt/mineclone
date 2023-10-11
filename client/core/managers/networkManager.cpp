#include "networkManager.hpp"

#include <iostream>

NetworkManager::NetworkManager() {
  if (enet_initialize () != 0) {
    std::cerr << "An error occurred while initializing ENet.\n" << std::endl;
    std::exit(1);
  }

  host = enet_host_create (NULL /* create a client host */,
              1 /* only allow 1 outgoing connection */,
              2 /* allow up 2 channels to be used, 0 and 1 */,
              0 /* assume any amount of incoming bandwidth */,
              0 /* assume any amount of outgoing bandwidth */);

  if (host == NULL) {
    std::cerr << "An error occurred while trying to create an ENet client host." << std::endl;
    std::exit(1);
  }
}

NetworkManager::~NetworkManager() {
  enet_host_destroy(host);
  enet_deinitialize();
}

bool NetworkManager::connect(std::string hostName, int port) {
  ENetAddress address;
  ENetEvent event;

  enet_address_set_host (&address, hostName.c_str());
  address.port = port;

  peer = enet_host_connect(host, &address, 2, 0);
  if (peer == NULL) {
    std::cerr << "No available peers for initiating an ENet connection." << std::endl;
    return false;
  }

  if (enet_host_service (host, &event, 100) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
    std::cout << "Connection to " << hostName << ":" << port << " succeeded." << std::endl;
    isConnected = true;
    return true;
  }

  enet_peer_reset (peer);
  std::cout << "Connection to " << hostName << ":" << port << " failed." << std::endl;
  return false;
}

void NetworkManager::disconnect() {
  enet_host_destroy(host);
  isConnected = false;
}

void NetworkManager::service() {
  if (!isConnected) return;

  ENetEvent event;
  while (enet_host_service(host, &event, 0) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        std::cout << "connected." << std::endl;
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        std::cout << "A packet of length " << event.packet->dataLength << " containing " << event.packet->data << " was received from " << event.peer->data << " on channel " << event.channelID << "." << std::endl;
        /* Clean up the packet now that we're done using it. */
        enet_packet_destroy(event.packet);
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        std::cout << "disconnected." << std::endl;
        /* Reset the peer's client information. */
        event.peer->data = NULL;
        break;
      case ENET_EVENT_TYPE_NONE:
        break;
    }
  }
}

bool NetworkManager::send(std::string str) {
  ENetPacket* packet = enet_packet_create(str.c_str(), str.size(), ENET_PACKET_FLAG_RELIABLE);
  int result = enet_peer_send(peer, 0, packet);
  return result == 0;
}
