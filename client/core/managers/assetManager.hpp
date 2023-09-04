#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "../graphics/font.hpp"
#include "../graphics/shader.hpp"
#include "../graphics/texture.hpp"

class AssetManager {
public:
  AssetManager();
  ~AssetManager();

  void load();
  void unload();

  TexturePtr getTexture(std::string name);
  ShaderPtr getShader(std::string name);
  FontPtr getFont(std::string name);

  static AssetManager* Instance() {
    if (instance == nullptr) {
      instance = new AssetManager();
    }
    return instance;
  }

private:
  std::map<std::string, TexturePtr> textures;
  std::map<std::string, ShaderPtr> shaders;
  std::map<std::string, FontPtr> fonts;

  static AssetManager* instance;
};
