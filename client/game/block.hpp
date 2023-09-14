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

  NOT_LOADED = 0xFF
};

bool isSolid(BlockType type);

// 2<     1<--0
// v \      \ ^
// 0-->1     >2

//   2----3
//  /|   /|
// 6----7 |     y
// | 0--|-1     ^
// |/   |/      |
// 4----5       ---> x
//             /
//            z

// 6 7 2 3 2 7
// 4 5 6 7 6 5
// 5 1 7 3 7 1
// 1 0 3 2 3 0
// 0 4 2 6 2 4
// 5 4 1 0 1 4
const std::vector<GLfloat> BLOCK_VERTEX_POSITIONS = {
  0, 1, 1,  1, 1, 1,  0, 1, 0, // Top
  1, 1, 0,  0, 1, 0,  1, 1, 1,
  0, 0, 1,  1, 0, 1,  0, 1, 1, // Front
  1, 1, 1,  0, 1, 1,  1, 0, 1,
  1, 0, 1,  1, 0, 0,  1, 1, 1, // Right
  1, 1, 0,  1, 1, 1,  1, 0, 0,
  1, 0, 0,  0, 0, 0,  1, 1, 0, // Back
  0, 1, 0,  1, 1, 0,  0, 0, 0,
  0, 0, 0,  0, 0, 1,  0, 1, 0, // Left
  0, 1, 1,  0, 1, 0,  0, 0, 1,
  1, 0, 1,  0, 0, 1,  1, 0, 0, // Bottom
  0, 0, 0,  1, 0, 0,  0, 0, 1
};

// fliped
// 2<-1    /0
// v /    / ^
// 0/    1->2

//   2----3
//  /|   /|
// 6----7 |     y
// | 0--|-1     ^
// |/   |/      |
// 4----5       ---> x
//             /
//            z

// 6 3 2 3 6 7
// 4 7 6 7 4 5
// 5 3 7 3 5 1
// 1 2 3 2 1 0
// 0 6 2 6 0 4
// 5 0 1 0 5 4
const std::vector<GLfloat> BLOCK_VERTEX_POSITIONS_FLIPPED = {
  0, 1, 1,  1, 1, 0,  0, 1, 0, // Top
  1, 1, 0,  0, 1, 1,  1, 1, 1,
  0, 0, 1,  1, 1, 1,  0, 1, 1, // Front
  1, 1, 1,  0, 0, 1,  1, 0, 1,
  1, 0, 1,  1, 1, 0,  1, 1, 1, // Right
  1, 1, 0,  1, 0, 1,  1, 0, 0,
  1, 0, 0,  0, 1, 0,  1, 1, 0, // Back
  0, 1, 0,  1, 0, 0,  0, 0, 0,
  0, 0, 0,  0, 1, 1,  0, 1, 0, // Left
  0, 1, 1,  0, 0, 0,  0, 0, 1,
  1, 0, 1,  0, 0, 0,  1, 0, 0, // Bottom
  0, 0, 0,  1, 0, 1,  0, 0, 1
};

// UV //
//
// 0-*-*-*-*-*-> x
// |T|F|R|B|L|b|
// v-*-*-*-*-*-*
// y
// Top Front Right Back Left bottom

const std::vector<GLfloat> BLOCK_VERTEX_UVS = {
  0, 1,  1, 1,  0, 0, // Top
  1, 0,  0, 0,  1, 1,
  1, 1,  2, 1,  1, 0, // Side
  2, 0,  1, 0,  2, 1,
  2, 1,  3, 1,  2, 0, // Bottom
  3, 0,  2, 0,  3, 1,
};

const std::vector<GLfloat> BLOCK_PREVIEW_LINES_VERTICES = {
  0, 0, 0,   0, 0, 1,
  0, 0, 1,   0, 1, 1,
  0, 1, 1,   0, 1, 0,
  0, 1, 0,   0, 0, 0,
  1, 0, 0,   1, 0, 1,
  1, 0, 1,   1, 1, 1,
  1, 1, 1,   1, 1, 0,
  1, 1, 0,   1, 0, 0,
  0, 0, 0,   1, 0, 0,
  0, 0, 1,   1, 0, 1,
  0, 1, 1,   1, 1, 1,
  0, 1, 0,   1, 1, 0,
};

const std::vector<GLfloat> BLOCK_PREVIEW_LINES_COLORS = {
  1, 1, 1,   1, 1, 1,
  1, 1, 1,   1, 1, 1,
  1, 1, 1,   1, 1, 1,
  1, 1, 1,   1, 1, 1,
  1, 1, 1,   1, 1, 1,
  1, 1, 1,   1, 1, 1,
  1, 1, 1,   1, 1, 1,
  1, 1, 1,   1, 1, 1,
  1, 1, 1,   1, 1, 1,
  1, 1, 1,   1, 1, 1,
  1, 1, 1,   1, 1, 1,
  1, 1, 1,   1, 1, 1,
};