#pragma once

#include "drawContext.hpp"
#include "../../common/byteStream.hpp"

#include <vector>

class Object {
public:
  Object();
  virtual ~Object();

  unsigned long long id;

  virtual void update(float dt);
  virtual void draw(DrawContext& drawCtx);

  virtual void serialize(DataWriteStream& stream) {}
  virtual void deserialize(DataReadStream& stream) {}
private:
};
