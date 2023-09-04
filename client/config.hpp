#pragma once

#include <SDL2/SDL.h>

const int MOUSE_TO_KEY_OFFSET = 500;

struct Config {
  SDL_Scancode forward = SDL_SCANCODE_W;
  SDL_Scancode backward = SDL_SCANCODE_S;
  SDL_Scancode left = SDL_SCANCODE_A;
  SDL_Scancode right = SDL_SCANCODE_D;
  SDL_Scancode jump = SDL_SCANCODE_SPACE;
  SDL_Scancode sprint = SDL_SCANCODE_LSHIFT;

  SDL_Scancode esc = SDL_SCANCODE_ESCAPE;
  SDL_Scancode debug = SDL_SCANCODE_0;
  SDL_Scancode menu = SDL_SCANCODE_TAB;

  SDL_Scancode placeBlock = (SDL_Scancode)(MOUSE_TO_KEY_OFFSET + SDL_BUTTON_RIGHT);
  SDL_Scancode breakBlock = (SDL_Scancode)(MOUSE_TO_KEY_OFFSET + SDL_BUTTON_LEFT);

  int width = 1280;
  int height = 720;

  bool saveConfig();
  static Config loadConfig();
};
