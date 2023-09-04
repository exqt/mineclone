#include "util.hpp"

int idiv(int x, int y) {
  return x >= 0 ? x/y : -((-x - 1) / y + 1);
}
