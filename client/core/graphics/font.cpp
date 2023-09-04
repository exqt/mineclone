#include "font.hpp"

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include "../managers/assetManager.hpp"

#include <glm/glm.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

Font::Font(std::string path) {
  stbtt_fontinfo font;

  std::ifstream fontFile(path, std::ios::binary);
  std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(fontFile), {});
  std::vector<unsigned char> bitmap(textureSize * textureSize, 0);

  stbtt_InitFont(&font, buffer.data(), stbtt_GetFontOffsetForIndex(buffer.data(), 0));

  int ascent, descent, lineGap;
  stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
  stbtt_ScaleForPixelHeight(&font, fontHeight);
  stbtt_BakeFontBitmap(buffer.data(), 0, fontHeight, bitmap.data(), textureSize, textureSize, 32, 96, cdata);

  std::vector<unsigned char> colorb(textureSize * textureSize * 4, 255);
  for (int i = 0; i < textureSize * textureSize; i++) {
    colorb[i * 4 + 3] = bitmap[i];
  }

  texture = std::make_shared<Texture>(textureSize, textureSize, GL_RGBA, colorb.data());
  mesh = std::make_shared<Mesh>();
}

Font::~Font() {
}

void Font::draw(DrawContext& ctx, std::string text, float x, float y) {
  std::vector<GLfloat> vertices;
  std::vector<GLfloat> uvs;

  float ox = x;
  float oy = y;
  float flipOffset = 2*oy;

  for (char c : text) {
    if (c == '\n') {
      x = ox;
      y += fontHeight;
      continue;
    }

    if (c >= 32) {
      stbtt_aligned_quad q;
      stbtt_GetBakedQuad(cdata, textureSize, textureSize, c - 32, &x, &y, &q, 1); //1=opengl & d3d10+,0=d3d9

      vertices.insert(vertices.end(), {
        q.x0, -q.y0 + flipOffset, 0.0f,
        q.x1, -q.y1 + flipOffset, 0.0f,
        q.x1, -q.y0 + flipOffset, 0.0f,

        q.x0, -q.y0 + flipOffset, 0.0f,
        q.x0, -q.y1 + flipOffset, 0.0f,
        q.x1, -q.y1 + flipOffset, 0.0f,
      });

      uvs.insert(uvs.end(), {
        q.s0, q.t0,
        q.s1, q.t1,
        q.s1, q.t0,

        q.s0, q.t0,
        q.s0, q.t1,
        q.s1, q.t1,
      });
    }
  }

  AssetManager* assetManager = AssetManager::Instance();
  auto shader = assetManager->getShader("main");
  shader->bind();

  glm::mat4 modelMat = glm::mat4(1.0f);
  glm::mat4 projViewMat = glm::ortho(0.0f, (float)ctx.width, 0.0f, (float)ctx.height);

  shader->setMat4("uModel", modelMat);
  shader->setMat4("uProjView", projViewMat);
  texture->bind();

  mesh->bind();
  mesh->update(vertices, uvs);
  mesh->draw();
}
