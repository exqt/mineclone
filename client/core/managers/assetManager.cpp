#include "assetManager.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <functional>

AssetManager* AssetManager::instance = nullptr;

AssetManager::AssetManager() {
}

AssetManager::~AssetManager() {
}

void loadRecursive(std::string path, std::string id, std::function<void(std::string, std::string)> callback) {
  for (auto &p : std::filesystem::directory_iterator(path)) {
    std::string path = p.path().string();
    std::string name = p.path().filename().string();

    if (p.is_directory()) {
      loadRecursive(path, id + name + "/", callback);
    }
    else {
      callback(path, id + name);
    }
  }
}

void AssetManager::load() {
  loadRecursive("assets/textures", "", [&](std::string path, std::string id) {
    if (path.find(".png") != std::string::npos) {
      textures[id] = std::make_shared<Texture>(path);
    }
  });

  loadRecursive("assets/shaders", "", [&](std::string path, std::string id) {
    if (path.find(".vert") != std::string::npos) {
      std::string vertPath = path;
      std::string fragPath = path;
      fragPath.replace(fragPath.find(".vert"), 5, ".frag");
      id = id.substr(0, id.find(".vert"));

      shaders[id] = std::make_shared<Shader>(vertPath, fragPath);
    }
  });

  loadRecursive("assets/fonts", "", [&](std::string path, std::string id) {
    if (path.find(".ttf") != std::string::npos) {
      fonts[id] = std::make_shared<Font>(path);
    }
  });
}

void AssetManager::unload() {

}

TexturePtr AssetManager::getTexture(std::string name) {
  if (textures.find(name) == textures.end()) {
    std::cerr << "Texture " << name << " not found." << std::endl;
    std::exit(-1);
  }
  return textures[name];
}

ShaderPtr AssetManager::getShader(std::string name) {
  if (shaders.find(name) == shaders.end()) {
    std::cerr << "Shader " << name << " not found." << std::endl;
    std::exit(-1);
  }
  return shaders[name];
}

FontPtr AssetManager::getFont(std::string name) {
  if (fonts.find(name) == fonts.end()) {
    std::cerr << "Font " << name << " not found." << std::endl;
    std::exit(-1);
  }
  return fonts[name];
}
