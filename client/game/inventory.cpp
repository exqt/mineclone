#include "inventory.hpp"

#include "../core/input.hpp"
#include "../core/managers/assetManager.hpp"

Inventory::Inventory() {
  blocks = {
    BlockType::DIRT_GRASS,
    BlockType::COLOR1,
    BlockType::COLOR2,
    BlockType::COLOR3,
    BlockType::COLOR4,
    BlockType::COLOR5,
    BlockType::COLOR6,
    BlockType::COLOR7,
    BlockType::COLOR8,
    BlockType::COLOR9,
    BlockType::COLOR10,
    BlockType::COLOR11,
    BlockType::COLOR12,
    BlockType::COLOR13,
    BlockType::COLOR14,
    BlockType::COLOR15,
  };
  mesh = std::make_shared<Mesh>();
  selectionQuadMesh = std::make_shared<Mesh>();

  const int sz = 32;
  selectionQuadMesh->update({
    0 , 0 , 0,
    0 , sz, 0,
    sz, 0 , 0,

    sz, 0 , 0,
    0 , sz, 0,
    sz, sz, 0,
  }, {
    240, 0,
    240, 16,
    240+16, 0,

    240+16, 0,
    240, 16,
    240+16, 16,
  });
}

Inventory::~Inventory() {

}

BlockType Inventory:: getCurrentBlock() {
  return blocks[currentIdx];
}

void Inventory::update(float dt) {
  auto& input = Input::Instance();

  if (input.getMouseWheel() > 0) {
    currentIdx = (currentIdx + 1) % blocks.size();
  }
  else if (input.getMouseWheel() < 0) {
    currentIdx = (currentIdx - 1 + blocks.size()) % blocks.size();
  }
}

void Inventory::draw(DrawContext& ctx) {
  auto assetManager = AssetManager::Instance();

  std::vector<float> vertices, uvs;
  int width = ctx.width;
  int height = ctx.height;

  const int sz = 32;
  for (int i = 0; i < (int)blocks.size(); i++) {
    auto block = blocks[i];
    vertices.insert(vertices.end(), {
      0  + 1.0f*i*sz, 0  + 1.0f, 0,
      0  + 1.0f*i*sz, sz + 1.0f, 0,
      sz + 1.0f*i*sz, 0  + 1.0f, 0,

      sz + 1.0f*i*sz, 0  + 1.0f, 0,
      0  + 1.0f*i*sz, sz + 1.0f, 0,
      sz + 1.0f*i*sz, sz + 1.0f, 0,
    });

    int blockOx = (block / 16) * 3 + 1;
    int blockOy = block % 16;
    float blockSize = 16;
    uvs.insert(uvs.end(), {
      blockOx*blockSize, blockOy*blockSize,
      blockOx*blockSize, (blockOy+1)*blockSize,
      (blockOx+1)*blockSize, blockOy*blockSize,

      (blockOx+1)*blockSize, blockOy*blockSize,
      blockOx*blockSize, (blockOy+1)*blockSize,
      (blockOx+1)*blockSize, (blockOy+1)*blockSize,
    });
  }

  mesh->update(vertices, uvs);

  auto shader = assetManager->getShader("ui");
  auto blocksTex = assetManager->getTexture("blocks.png");

  shader->bind();
  shader->setVec2("uResolution", glm::vec2(width, height));
  shader->setVec2("uTextureSize", glm::vec2(blocksTex->width, blocksTex->height));
  shader->setVec2("uOffset", glm::vec2(width/2 - 32*8, height - 32));

  blocksTex->bind();
  mesh->draw();

  shader->setVec2("uOffset", glm::vec2(width/2 - 32*8, height - 32) + glm::vec2(currentIdx*32, 0));
  selectionQuadMesh->draw();
}
