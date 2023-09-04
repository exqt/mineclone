#include "infoView.hpp"

#include <SDL2/SDL.h>
#include <string>

InfoView::InfoView() {
}

InfoView::~InfoView() {
}

void InfoView::draw(DebugInfo &debugInfo) {
  ImGui::Begin("Info View");

  ImGui::Text("FPS: %d", debugInfo.fps);
  ImGui::Text("VSync: %d", debugInfo.vsync);

  ImGui::Text("Player Position: (%f, %f, %f)", debugInfo.playerPos.x, debugInfo.playerPos.y, debugInfo.playerPos.z);

  ImGui::End();
}
