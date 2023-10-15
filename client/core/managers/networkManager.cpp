#include "networkManager.hpp"

#include <iostream>

void NetworkManager::onConnect(ENetEvent& event) {
  std::cout << "connected" << std::endl;
  event.peer->data = (void*)"Test";
}

void NetworkManager::onReceive(ENetEvent& event, RPCHandler rpc, ObjectSyncHandler sync) {
  auto data = std::vector<std::byte>((std::byte*)event.packet->data, (std::byte*)event.packet->data + event.packet->dataLength);
  auto stream = DataReadStream();
  stream.data = data;

  auto type = stream.pop<GamePacketDataType>();
  if (type == GamePacketDataType::RPC_RESPONSE) {
    std::string methodName = stream.popString();
    rpc(methodName, stream);
  }
  else if (type == GamePacketDataType::GAME_OBJECT_UPDATE) {
    ObjectId id = stream.pop<ObjectId>();
    sync(id, stream);
  }
  else {
    std::cout << "Unknown packet type" << std::endl;
  }

  enet_packet_destroy(event.packet);

}

void NetworkManager::onDisconnect(ENetEvent& event) {
  std::cout << "disconnected" << std::endl;
  event.peer->data = NULL;
}

void NetworkManager::service(RPCHandler rpc, ObjectSyncHandler sync) {
  if (!isConnected) return;

  ENetEvent event;
  while (enet_host_service(host, &event, 0) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        onConnect(event);
        break;

      case ENET_EVENT_TYPE_RECEIVE:
        onReceive(event, rpc, sync);
        break;

      case ENET_EVENT_TYPE_DISCONNECT:
        onDisconnect(event);
        break;

      case ENET_EVENT_TYPE_NONE:
        break;
    }
  }
}

void NetworkManager::rpcCall(std::string name, DataStream& stream) {
  DataWriteStream writeStream;
  writeStream.push(GamePacketDataType::RPC_CALL);
  writeStream.pushString(name);
  writeStream.data.insert(writeStream.data.end(), stream.data.begin(), stream.data.end());

  ENetPacket* packet = enet_packet_create(writeStream.data.data(), writeStream.data.size(), ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(peer, 0, packet);
}

void NetworkManager::syncObject(ObjectId id, DataStream& stream) {
  DataWriteStream writeStream;
  writeStream.push(GamePacketDataType::GAME_OBJECT_UPDATE);
  writeStream.push(id);
  writeStream.data.insert(writeStream.data.end(), stream.data.begin(), stream.data.end());

  ENetPacket* packet = enet_packet_create(writeStream.data.data(), writeStream.data.size(), ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(peer, 0, packet);
}

bool NetworkManager::connect(std::string hostName, int port) {
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
  enet_deinitialize();
  isConnected = false;
}
