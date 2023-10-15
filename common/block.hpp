#pragma once

#include <cstdint>
#include <vector>
#include <GL/glew.h>
#include <tuple>
#include <optional>

enum BlockType : unsigned char {
  NONE,
  DIRT_GRASS,
  DIRT,
  STONE,
  SAND,
  BEDROCK,
  WATER,
  TREE_TRUNK,
  TREE_LEAVES,
  TREE_LEAVES_SAKURA,
  GLASS,

  COLOR0,
  COLOR1,
  COLOR2,
  COLOR3,
  COLOR4,
  COLOR5,
  COLOR6,
  COLOR7,
  COLOR8,
  COLOR9,
  COLOR10,
  COLOR11,

  COMPRESSING_BYTE = 0xFE,
  NOT_LOADED = 0xFF
};

#define IS_SOLID(block) (block != BlockType::NONE && block != BlockType::WATER && block != BlockType::NOT_LOADED)
