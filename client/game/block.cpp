#include "block.hpp"

bool isSolid(BlockType block) {
  switch (block) {
    case BlockType::NONE:
    case BlockType::WATER:
    case BlockType::SEA_WATER:
    case BlockType::NOT_LOADED:
      return false;
    default:
      return true;
  }
}
