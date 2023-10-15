#pragma once

extern "C" {
  #include <enet/enet.h>
}

#include <vector>
#include <string>
#include <vector>
#include <functional>
#include "../common/byteStream.hpp"
#include "../common/gamePacket.hpp"

using RPCHandler = std::function<void(std::string, DataReadStream&)>;
using ObjectSyncHandler = std::function<void(ObjectId, DataReadStream&)>;

class NetworkManager {
public:
  static NetworkManager& Instance() {
    static NetworkManager instance;
    return instance;
  }

  NetworkManager() {}
  ~NetworkManager() {}

  bool connect(std::string hostName, int port);
  void disconnect();
  void service(RPCHandler rpc, ObjectSyncHandler sync);

  void rpcCall(std::string name, DataStream& stream);
  void syncObject(ObjectId id, DataStream& stream);

private:
  ENetHost* host;
  ENetPeer* peer;
  bool isConnected;

  void onConnect(ENetEvent& event);
  void onReceive(ENetEvent& event, RPCHandler rpc, ObjectSyncHandler sync);
  void onDisconnect(ENetEvent& event);
};
