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
    stream.push<std::string>(type);
    stream.push<NetworkObjectOwner>(owner);
    stream.push<std::vector<std::byte>>(data);
    return stream.data;
  }

  static NetworkObjectData fromByteArray(std::vector<std::byte>& data) {
    DataReadStream stream;
    stream.data = data;

    NetworkObjectData objData;
    objData.id = stream.pop<NetworkObjectId>();
    objData.type = stream.pop<std::string>();
    objData.owner = stream.pop<NetworkObjectOwner>();
    objData.data = stream.pop<std::vector<std::byte>>();

    return objData;
  }
};
