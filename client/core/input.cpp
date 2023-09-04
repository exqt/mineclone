#include "input.hpp"
#include "../config.hpp"

Input::Input() {
}

Input::~Input() {
}

void Input::inputKeyState() {
  _keyState = SDL_GetKeyboardState(nullptr);
}

void Input::clear() {
  _keyPressed.reset();
  _keyReleased.reset();
  _mouseDx = 0; _mouseDy = 0;
}

void Input::inputKeyboard(SDL_KeyboardEvent e) {
  if (e.repeat) return;
  if (e.type == SDL_KEYDOWN){
    _keyPressed[e.keysym.scancode] = true;
  }
  else if (e.type == SDL_KEYUP) {
    _keyReleased[e.keysym.scancode] = true;
  }
}

void Input::inputMouseMotion(SDL_MouseMotionEvent e) {
  _mouseX = e.x;
  _mouseY = e.y;
  _mouseDx += e.xrel;
  _mouseDy += e.yrel;
}

void Input::inputMouseButton(SDL_MouseButtonEvent e) {
  if (e.button > 5) return;
  if (e.type == SDL_MOUSEBUTTONDOWN) {
    _keyPressed[e.button + MOUSE_TO_KEY_OFFSET] = true;
  }
  else if (e.type == SDL_MOUSEBUTTONUP) {
    _keyReleased[e.button + MOUSE_TO_KEY_OFFSET] = true;
  }
}

bool Input::isKeyPressed(SDL_Scancode cd) {
  return _keyPressed[cd];
}

bool Input::isKeyReleased(SDL_Scancode cd) {
  return _keyReleased[cd];
}

bool Input::isKeyDown(SDL_Scancode cd) {
  return _keyState[cd];
}

MouseVec2 Input::getMouseMove() {
  return {_mouseDx, _mouseDy};
}

MouseVec2 Input::getMousePosition() {
  return {_mouseX, _mouseY};
}
