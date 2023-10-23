#pragma once

#include "byteStream.hpp"

using NetworkObjectId = unsigned long long;
using NetworkObjectOwner = unsigned short;

struct NetworkObjectData {
  NetworkObjectId id;
  std::string type;
  NetworkObjectOwner owner;
  std::vector<std::byte> data;

  std::vector<std::byte> toByteArray() {
    DataWriteStream stream;
    stream.push<NetworkObjectId>(id);
    stream.pushString(type);
    stream.push<NetworkObjectOwner>(owner);
    stream.pushVector(data);
    return stream.data;
  }

  static NetworkObjectData fromByteArray(std::vector<std::byte> data) {
    DataReadStream stream;
    stream.data = data;

    NetworkObjectData objData;
    objData.id = stream.pop<NetworkObjectId>();
    objData.type = stream.popString();
    objData.owner = stream.pop<NetworkObjectOwner>();
    objData.data = stream.popVector<std::byte>();

    return objData;
  }
};
