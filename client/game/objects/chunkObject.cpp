#include "chunkObject.hpp"

#include "../../core/graphics/drawCommand.hpp"
#include "../../core/managers/assetManager.hpp"
#include "../meshData.hpp"

/* Chunk Mesh */
ChunkMesh::ChunkMesh() {
  glGenBuffers(1, &vboMetadata);
}

ChunkMesh::~ChunkMesh() {
  glDeleteBuffers(1, &vboMetadata);
}

void ChunkMesh::update(std::vector<float> vertices, std::vector<float> uvs, std::vector<uint32_t> metadata) {
  Mesh::update(vertices, uvs);

  glBindBuffer(GL_ARRAY_BUFFER, vboMetadata);
  glBufferData(GL_ARRAY_BUFFER, metadata.size() * sizeof(uint32_t), metadata.data(), GL_STATIC_DRAW);
  glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 0, (void*)0);
  glEnableVertexAttribArray(2);
}

/* Chunk Object */
ChunkObject::ChunkObject(World* world, int ox, int oy, int oz) {
  this->world = world;
  this->ox = ox;
  this->oy = oy;
  this->oz = oz;

  mesh = std::make_unique<ChunkMesh>();
  waterMesh = std::make_shared<Mesh>();
}

ChunkObject::~ChunkObject() {
}

BlockType ChunkObject::getBlock(int x, int y, int z) {
  if (0 <= x && x < ChunkData::BLOCKS_X &&
      0 <= y && y < ChunkData::BLOCKS_Y &&
      0 <= z && z < ChunkData::BLOCKS_Z)
  {
    return chunkData->get(x, y, z);
  }
  else return world->getBlock(
    x + ox * ChunkData::BLOCKS_X,
    y + oy * ChunkData::BLOCKS_Y,
    z + oz * ChunkData::BLOCKS_Z
  );
}

void ChunkObject::setChunkData(ChunkDataPtr chunkData) {
  this->chunkData = chunkData;
}

void ChunkObject::update(float dt) {
}

void ChunkObject::draw(DrawContext& ctx) {
  if (chunkData == nullptr) return;

  auto shader = AssetManager::Instance()->getShader("block");
  shader->bind();
  shader->setMat4("uProjView", ctx.camera->getProjectionViewMatrix());
  shader->setVec3("uOffset", glm::vec3(ox * ChunkData::BLOCKS_X, oy * ChunkData::BLOCKS_Y, oz * ChunkData::BLOCKS_Z));

  auto texture = AssetManager::Instance()->getTexture("blocks.png");
  texture->bind();

  mesh->draw();

  // water

  auto waterShader = AssetManager::Instance()->getShader("water");
  // waterShader->bind();
  // waterShader->setMat4("uProjView", ctx.camera->getProjectionViewMatrix());
  // waterShader->setVec3("uOffset", glm::vec3(ox*ChunkData::BLOCKS_X, oy*ChunkData::BLOCKS_Y, oz*ChunkData::BLOCKS_Z));
  // waterMesh->draw();

  DrawCommand waterDrawCommand;
  waterDrawCommand.mesh = waterMesh;
  waterDrawCommand.shader = waterShader;
  waterDrawCommand.shaderParams = {
    {"uProjView", ctx.camera->getProjectionViewMatrix()},
    {"uOffset", glm::vec3(ox*ChunkData::BLOCKS_X, oy*ChunkData::BLOCKS_Y, oz*ChunkData::BLOCKS_Z)}
  };
  ctx.drawCommands.push_back(waterDrawCommand);
}

bool isTransparent(BlockType block) {
  return block == NONE || block == WATER || block == GLASS || block == TREE_LEAVES;
}

bool shouldApplyAO(BlockType block) {
  return block != NONE && block != WATER && block != GLASS;
}

GLuint vertexAO(GLuint side1, GLuint side2, GLuint corner) {
  return side1 + side2 + std::max(corner, side1 & side2);
}

void ChunkObject::buildMesh() {
  std::vector<float> positions;
  std::vector<float> uvs;
  std::vector<GLuint> metadata;

  for (int ix = 0; ix < ChunkData::BLOCKS_X; ix++) {
    for (int iy = 0; iy < ChunkData::BLOCKS_Y; iy++) {
      for (int iz = 0; iz < ChunkData::BLOCKS_Z; iz++) {
        BlockType block = chunkData->get(ix, iy, iz);
        if (block == NONE) continue;
        if (block == WATER) continue;

        int visibles[6] = {
          isTransparent(getBlock(ix, iy + 1, iz)), // top
          isTransparent(getBlock(ix, iy, iz + 1)), // front
          isTransparent(getBlock(ix + 1, iy, iz)), // right
          isTransparent(getBlock(ix, iy, iz - 1)), // back
          isTransparent(getBlock(ix - 1, iy, iz)), // left
          isTransparent(getBlock(ix, iy - 1, iz)) // bottom
        };

        if (isTransparent(block)) {
          if (visibles[0] && getBlock(ix, iy + 1, iz) == block) visibles[0] = false;
          if (visibles[1] && getBlock(ix, iy, iz + 1) == block) visibles[1] = false;
          if (visibles[2] && getBlock(ix + 1, iy, iz) == block) visibles[2] = false;
          if (visibles[3] && getBlock(ix, iy, iz - 1) == block) visibles[3] = false;
          if (visibles[4] && getBlock(ix - 1, iy, iz) == block) visibles[4] = false;
          if (visibles[5] && getBlock(ix, iy - 1, iz) == block) visibles[5] = false;
        }

        // https://0fps.net/2013/07/03/ambient-occlusion-for-minecraft-like-worlds/
        for (int iFace = 0; iFace < 6; iFace++) { // for each face
          if (!visibles[iFace]) continue;

          GLuint v00 = 0, v01 = 0, v10 = 0, v11 = 0;
          GLuint a00 = 0, a01 = 0, a10 = 0, a11 = 0;

          if (shouldApplyAO(getBlock(ix, iy, iz))) {
            GLuint side0, side1, side2, side3, corner00, corner01, corner10, corner11;
            if (iFace == 0) { // Top
              side0 = shouldApplyAO(getBlock(ix    , iy + 1, iz - 1)); // up
              side1 = shouldApplyAO(getBlock(ix + 1, iy + 1, iz    )); // right
              side2 = shouldApplyAO(getBlock(ix    , iy + 1, iz + 1)); // down
              side3 = shouldApplyAO(getBlock(ix - 1, iy + 1, iz    )); // left
              corner00 = shouldApplyAO(getBlock(ix - 1, iy + 1, iz - 1));
              corner01 = shouldApplyAO(getBlock(ix + 1, iy + 1, iz - 1));
              corner10 = shouldApplyAO(getBlock(ix - 1, iy + 1, iz + 1));
              corner11 = shouldApplyAO(getBlock(ix + 1, iy + 1, iz + 1));
            }
            else if (iFace == 1) { // Front
              side0 = shouldApplyAO(getBlock(ix    , iy + 1, iz + 1)); // up
              side1 = shouldApplyAO(getBlock(ix + 1, iy    , iz + 1)); // right
              side2 = shouldApplyAO(getBlock(ix    , iy - 1, iz + 1)); // down
              side3 = shouldApplyAO(getBlock(ix - 1, iy    , iz + 1)); // left
              corner00 = shouldApplyAO(getBlock(ix - 1, iy + 1, iz + 1));
              corner01 = shouldApplyAO(getBlock(ix + 1, iy + 1, iz + 1));
              corner10 = shouldApplyAO(getBlock(ix - 1, iy - 1, iz + 1));
              corner11 = shouldApplyAO(getBlock(ix + 1, iy - 1, iz + 1));
            }
            else if (iFace == 2) { // Right
              side0 = shouldApplyAO(getBlock(ix + 1, iy + 1, iz    )); // up
              side1 = shouldApplyAO(getBlock(ix + 1, iy    , iz - 1)); // right
              side2 = shouldApplyAO(getBlock(ix + 1, iy - 1, iz    )); // down
              side3 = shouldApplyAO(getBlock(ix + 1, iy    , iz + 1)); // left
              corner00 = shouldApplyAO(getBlock(ix + 1, iy + 1, iz + 1));
              corner01 = shouldApplyAO(getBlock(ix + 1, iy + 1, iz - 1));
              corner10 = shouldApplyAO(getBlock(ix + 1, iy - 1, iz + 1));
              corner11 = shouldApplyAO(getBlock(ix + 1, iy - 1, iz - 1));
            }
            else if (iFace == 3) { // Back
              side0 = shouldApplyAO(getBlock(ix    , iy + 1, iz - 1)); // up
              side1 = shouldApplyAO(getBlock(ix - 1, iy    , iz - 1)); // right
              side2 = shouldApplyAO(getBlock(ix    , iy - 1, iz - 1)); // down
              side3 = shouldApplyAO(getBlock(ix + 1, iy    , iz - 1)); // left
              corner00 = shouldApplyAO(getBlock(ix + 1, iy + 1, iz - 1));
              corner01 = shouldApplyAO(getBlock(ix - 1, iy + 1, iz - 1));
              corner10 = shouldApplyAO(getBlock(ix + 1, iy - 1, iz - 1));
              corner11 = shouldApplyAO(getBlock(ix - 1, iy - 1, iz - 1));
            }
            else if (iFace == 4) { // Left
              side0 = shouldApplyAO(getBlock(ix - 1, iy + 1, iz    )); // up
              side1 = shouldApplyAO(getBlock(ix - 1, iy    , iz + 1)); // right
              side2 = shouldApplyAO(getBlock(ix - 1, iy - 1, iz    )); // down
              side3 = shouldApplyAO(getBlock(ix - 1, iy    , iz - 1)); // left
              corner00 = shouldApplyAO(getBlock(ix - 1, iy + 1, iz - 1));
              corner01 = shouldApplyAO(getBlock(ix - 1, iy + 1, iz + 1));
              corner10 = shouldApplyAO(getBlock(ix - 1, iy - 1, iz - 1));
              corner11 = shouldApplyAO(getBlock(ix - 1, iy - 1, iz + 1));
            }
            else { // Bottom
              side0 = shouldApplyAO(getBlock(ix    , iy - 1, iz - 1)); // up
              side1 = shouldApplyAO(getBlock(ix - 1, iy - 1, iz    )); // right
              side2 = shouldApplyAO(getBlock(ix    , iy - 1, iz + 1)); // down
              side3 = shouldApplyAO(getBlock(ix + 1, iy - 1, iz    )); // left
              corner00 = shouldApplyAO(getBlock(ix + 1, iy - 1, iz - 1));
              corner01 = shouldApplyAO(getBlock(ix - 1, iy - 1, iz - 1));
              corner10 = shouldApplyAO(getBlock(ix + 1, iy - 1, iz + 1));
              corner11 = shouldApplyAO(getBlock(ix - 1, iy - 1, iz + 1));
            }
            a00 = vertexAO(side3, side0, corner00);
            a01 = vertexAO(side0, side1, corner01);
            a10 = vertexAO(side2, side3, corner10);
            a11 = vertexAO(side1, side2, corner11);
          }

          GLuint quadAO = (a00 << 0) | (a01 << 2) | (a10 << 4) | (a11 << 6);
            v00 = quadAO;
            v01 = quadAO;
            v10 = quadAO;
            v11 = quadAO;

          for (int j = 0; j < 6; j++) { // add vec3 * 6 positions
            positions.push_back(BLOCK_VERTEX_POSITIONS[3*6*iFace + 3*j + 0] + ix);
            positions.push_back(BLOCK_VERTEX_POSITIONS[3*6*iFace + 3*j + 1] + iy);
            positions.push_back(BLOCK_VERTEX_POSITIONS[3*6*iFace + 3*j + 2] + iz);
          }

          int uvFace = iFace == 5 ? 2 : (iFace == 0 ? 0 : 1);
          int uvOffsetX = (block / 16) * 3;
          int uvOffsetY = block % 16;
          for (int j = 0; j < 6; j++) { // add vec2 * 6 uvs
            uvs.push_back(BLOCK_VERTEX_UVS[2*6*uvFace + 2*j + 0] + uvOffsetX);
            uvs.push_back(BLOCK_VERTEX_UVS[2*6*uvFace + 2*j + 1] + uvOffsetY);
          }

          metadata.insert(metadata.end(), {v10, v01, v00, v01, v10, v11});
        }
      }
    }
  }

  mesh->update(positions, uvs, metadata);

  // water
  std::vector<float> waterPositions;
  std::vector<float> waterUvs;

  for (int ix = 0; ix < ChunkData::BLOCKS_X; ix++) {
    for (int iy = 0; iy < ChunkData::BLOCKS_Y; iy++) {
      for (int iz = 0; iz < ChunkData::BLOCKS_Z; iz++) {
        BlockType block = chunkData->get(ix, iy, iz);
        if (block != WATER) continue;

        BlockType adjBlocks[6] = {
          getBlock(ix, iy + 1, iz), // top
          getBlock(ix, iy, iz + 1), // front
          getBlock(ix + 1, iy, iz), // right
          getBlock(ix, iy, iz - 1), // back
          getBlock(ix - 1, iy, iz), // left
          getBlock(ix, iy - 1, iz) // bottom
        };

        for (int iFace = 0; iFace < 6; iFace++) { // for each face
          if (adjBlocks[iFace] == WATER || !isTransparent(adjBlocks[iFace])) continue;

          for (int j = 0; j < 6; j++) { // add vec3 * 6 positions
            waterPositions.push_back(BLOCK_VERTEX_POSITIONS[3*6*iFace + 3*j + 0] + ix);
            waterPositions.push_back(BLOCK_VERTEX_POSITIONS[3*6*iFace + 3*j + 1] + iy);
            waterPositions.push_back(BLOCK_VERTEX_POSITIONS[3*6*iFace + 3*j + 2] + iz);
            waterUvs.push_back(0);
            waterUvs.push_back(0);
          }
        }
      }
    }
  }

  waterMesh->update(waterPositions, waterUvs);
}

void ChunkObject::serialize(DataWriteStream& stream) {
  stream.push<int>(ox);
  stream.push<int>(oy);
  stream.push<int>(oz);
  stream.pushVector(chunkData->toByteArray());
}

void ChunkObject::deserialize(DataReadStream& stream) {
  ox = stream.pop<int>();
  oy = stream.pop<int>();
  oz = stream.pop<int>();
  chunkData->copyFromByteArray(stream.popVector<std::byte>());
}
