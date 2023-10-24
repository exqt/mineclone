#pragma once

#include <cstring>
#include <map>
#include <bitset>
#include <SDL2/SDL.h>

struct MouseVec2 {
  int x, y;
};

class Input
{
public:
  static Input& Instance() {
    static Input instance;
    return instance;
  }

  void inputKeyState();
  void inputMouseMotion(SDL_MouseMotionEvent e);
  void inputMouseButton(SDL_MouseButtonEvent e);
  void inputMouseWheel(SDL_MouseWheelEvent e);
  void inputKeyboard(SDL_KeyboardEvent e);

  void clear();

  //
  bool isKeyPressed(SDL_Scancode cd);
  bool isKeyReleased(SDL_Scancode cd);
  bool isKeyDown(SDL_Scancode cd);
  MouseVec2 getMouseMove();
  MouseVec2 getMousePosition();
  int getMouseWheel();

private:
  Input();
  ~Input();

  std::bitset<512> _keyPressed, _keyReleased;
  const Uint8* _keyState;
  int _mouseX, _mouseY, _mouseDx, _mouseDy;
  int _mouseWheel;
};
