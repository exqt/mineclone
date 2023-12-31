#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <vector>
#include <map>
#include <unordered_set>
#include <utility>
#include <tuple>
#include <memory>

#include "../../../common/chunkData.hpp"
#include "../../../common/world.hpp"
#include "../collision.hpp"
#include "../../core/object.hpp"
#include "../../core/graphics/shader.hpp"
#include "../../core/graphics/texture.hpp"
#include "../../core/graphics/mesh.hpp"
#include "../chunkMeshBuildQueue.hpp"

class ChunkMesh : public Mesh {
public:
  ChunkMesh();
  ~ChunkMesh();

  void update(std::vector<float> vertices, std::vector<float> uvs, std::vector<uint32_t> metadata);

  GLuint vboMetadata;
};

class ChunkObject : public Object {
public:
  ChunkObject(World* world, int ox, int oy, int oz);
  ~ChunkObject() override;

  int ox, oy, oz;
  ChunkDataPtr chunkData;
  World *world;

  CollisionMap* collisionMap;
  ChunkMeshBuildQueue* meshBuildQueue;

  void buildMesh();

  void setChunkData(ChunkDataPtr chunkData);

  void update(float dt);
  void draw(DrawContext& ctx);

  void serialize(DataWriteStream& stream) override;
  void deserialize(DataReadStream& stream) override;

  virtual std::string getNetworkType() { return "CHUNK"; }

private:
  std::unique_ptr<ChunkMesh> mesh;
  MeshPtr waterMesh;

  BlockType getBlock(int x, int y, int z);
};

using ChunkObjectPtr = std::shared_ptr<ChunkObject>;
