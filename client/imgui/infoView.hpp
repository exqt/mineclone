#pragma once

#include <imgui/imgui.h>

#include <glm/glm.hpp>

struct DebugInfo {
  int fps;
  int vsync;
  glm::vec3 playerPos;

  static DebugInfo& get() {
    static DebugInfo instance;
    return instance;
  }
};

class InfoView {
public:
  InfoView();
  ~InfoView();

  void draw(DebugInfo& debugInfo);

private:
};
