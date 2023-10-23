#pragma once

#include "../core/graphics/drawContext.hpp"
#include "../../common/byteStream.hpp"

#include <vector>
#include <string>

class Object {
public:
  Object() {}
  virtual ~Object() {};

  virtual void update(float dt) = 0;
  virtual void draw(DrawContext& drawCtx) = 0;

  virtual void serialize(DataWriteStream& stream) = 0;
  virtual void deserialize(DataReadStream& stream) = 0;

  virtual std::string getNetworkType() = 0;
private:
};
