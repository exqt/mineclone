#pragma once

#include <GL/glew.h>    // Include GLEW before any other OpenGL headers
#include <GL/gl.h>      // Include OpenGL headers after GLEW
#include <GL/glu.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <queue>
#include <atomic>
#include <string>

#include "imgui/infoView.hpp"

class Application {
public:
  Application();
  ~Application();

  void init();
  void connect(std::string host, int port, std::string name);
  void clean();
  void run();

private:
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_GLContext glContext;

  InfoView *infoView;
};
