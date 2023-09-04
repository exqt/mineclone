#include "application.hpp"

int main(int argc, char* args[]) {
  Application app;
  app.init();
  app.run();
  app.clean();

  return 0;
}
