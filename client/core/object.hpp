#pragma once

#include "drawContext.hpp"

#include <vector>

class Object {
public:
  Object();
  virtual ~Object();

  virtual void update(float dt);
  virtual void draw(DrawContext&drawCtx);
private:
};
