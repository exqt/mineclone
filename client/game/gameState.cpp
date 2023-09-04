#include "gameState.hpp"

#include <SDL2/SDL.h>

GameState::GameState() {
}

void GameState::setMenuOpen(bool open) {
  _isMenuOpen = open;

  if (_isMenuOpen) {
    SDL_SetRelativeMouseMode(SDL_FALSE);
  } else {
    SDL_SetRelativeMouseMode(SDL_TRUE);
  }
}

bool GameState::isMenuOpen() {
  return _isMenuOpen;
}
