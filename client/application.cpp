#include "application.hpp"

#include <iostream>
#include <atomic>
#include <thread>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "game.hpp"
#include "core/input.hpp"
#include "core/managers/assetManager.hpp"

#ifdef DEBUG
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#endif

#include "core/managers/networkManager.hpp"

Application::Application() {
}

Application::~Application() {
}

void Application::init() {
  Config config;
  // NetworkManager& networkManager = NetworkManager::Instance();
  // bool success = networkManager.connect("127.0.0.1", 7878);

 if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cerr << "SDL could not initialize! SDL Error:" << SDL_GetError() << std::endl;
    std::exit(-1);
  }

  window = SDL_CreateWindow("Mineclone", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, config.width, config.height, SDL_WINDOW_OPENGL);
  if (window == NULL) {
    std::cerr << "Window could not be created! SDL Error:" << SDL_GetError() << std::endl;
    std::exit(-1);
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    std::cerr << "Renderer could not be created! SDL Error:" << SDL_GetError() << std::endl;
    std::exit(-1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

  glContext = SDL_GL_CreateContext(window);
  if (glContext == NULL) {
    std::cerr << "OpenGL context could not be created! SDL Error:" << SDL_GetError() << std::endl;
    std::exit(-1);
  }

  if (glewInit() != GLEW_OK) {
    std::cerr << "GLEW could not be initialized!" << std::endl;
    std::exit(-1);
  }

  SDL_SetWindowResizable(window, SDL_TRUE);

#ifdef DEBUG
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, glContext);
  ImGui_ImplOpenGL3_Init();
#endif

  AssetManager::Instance()->load();
}

#include <iostream>

void Application::run() {
  SDL_GL_SetSwapInterval(1);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  Uint64 lastTime = SDL_GetTicks64();

  Input& input = Input::Instance();
  NetworkManager& networkManager = NetworkManager::Instance();

  bool shouldQuit = false;
  bool debug = false;

  Config config;
  Game game;
  game.setGameSize(config.width, config.height);

  while (!shouldQuit) {
    Uint64 currentTime = SDL_GetTicks64();
    int dt = std::min(currentTime - lastTime, 100uLL);
    lastTime = currentTime;

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
#ifdef DEBUG
      if (debug) ImGui_ImplSDL2_ProcessEvent(&e);
#endif
      switch (e.type) {
        case SDL_MOUSEMOTION:
          input.inputMouseMotion(e.motion); break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
          input.inputMouseButton(e.button); break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
          input.inputKeyboard(e.key); break;
        case SDL_WINDOWEVENT:
          if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
            int width = e.window.data1;
            int height = e.window.data2;
            game.setGameSize(width, height);
            glViewport(0, 0, width, height);
          }
          break;
        case SDL_QUIT:
          shouldQuit = true;
          break;
        default:
          break;
      }
    }

    input.inputKeyState();

    if (input.isKeyPressed(config.esc)) shouldQuit = true;
    if (input.isKeyPressed(config.debug)) debug = !debug;

    networkManager.service();

    game.update(dt/1000.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    game.draw();

#ifdef DEBUG
    DebugInfo& debugInfo = DebugInfo::get();
    debugInfo.fps = 1000.0 / dt;
    debugInfo.vsync = SDL_GL_GetSwapInterval();

    if (debug) {
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL2_NewFrame();
      ImGui::NewFrame();
      ImGui::ShowDemoWindow(); // Show demo window! :)

      infoView->draw(debugInfo);

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
#endif

    SDL_GL_SwapWindow(window);

    input.clear();

    SDL_Delay(1);
  }
}

void Application::clean() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

#ifdef DEBUG
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
#endif
}
