#pragma once

#include <string>
#include <memory>

#include "texture.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "drawContext.hpp"

#include <stb_truetype.h>

class Font {
public:
  Font(std::string path);
  ~Font();

  void draw(DrawContext&ctx, std::string text, float x, float y);

private:
  int textureSize = 256;
  int fontHeight = 24;
  TexturePtr texture;
  MeshPtr mesh;
  stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
};

using FontPtr = std::shared_ptr<Font>;
