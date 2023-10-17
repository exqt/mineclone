#pragma once

#include "byteStream.hpp"

using NetworkObjectId = unsigned long long;
using NetworkObjectOwner = unsigned short;

struct NetworkObject {
  NetworkObjectId id;
  std::string type;
  NetworkObjectOwner owner;
  std::vector<std::byte> data;

  std::vector<std::byte> toByteArray() {
    DataWriteStream stream;
    stream.push<NetworkObjectId>(id);
    stream.push<std::string>(type);
    stream.push<NetworkObjectOwner>(owner);
    stream.pushVector(data);
    return stream.data;
  }

  static NetworkObject fromByteArray(std::vector<std::byte> data) {
    DataReadStream stream;
    stream.data = data;

    return {
      .id = stream.pop<NetworkObjectId>(),
      .type = stream.pop<std::string>(),
      .owner = stream.pop<NetworkObjectOwner>(),
      .data = stream.popVector<std::byte>()
    };
  }
};
