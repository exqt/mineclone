#include "skyDome.hpp"

#include "../../core/managers/assetManager.hpp"

SkyDome::SkyDome() {
  mesh = new Mesh();
  mesh->update({
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
  }, {});
}

SkyDome::~SkyDome() {
}

void SkyDome::setTime(float time) {
  this->time = time;
}

void SkyDome::draw(DrawContext& ctx) {
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  auto shader = AssetManager::Instance()->getShader("skyDome");
  shader->bind();
  shader->setFloat("uTime", time);

  glm::vec3 positionBackup = ctx.camera->position;
  ctx.camera->position = glm::vec3(0.0f);
  shader->setMat4("uProjView", ctx.camera->getProjectionViewMatrix());
  shader->setVec3("uCameraPosition", ctx.camera->position);
  ctx.camera->position = positionBackup;

  mesh->draw();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}
