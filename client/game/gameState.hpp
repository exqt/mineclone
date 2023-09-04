#pragma once

class GameState {
public:
  static GameState& Instance() {
    static GameState instance;
    return instance;
  }

  GameState();

  bool isMenuOpen();
  void setMenuOpen(bool open);

private:
  bool _isMenuOpen = false;
};
