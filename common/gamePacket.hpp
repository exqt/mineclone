#pragma once

enum GamePacketDataType : unsigned char {
  RPC_CALL,
  RPC_RESPONSE,

  GAME_OBJECT_CREATE,
  GAME_OBJECT_UPDATE,
  GAME_OBJECT_DESTROY,
};

using ObjectId = unsigned long long;
using ObjectClass = unsigned short;
