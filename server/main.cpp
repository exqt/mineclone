#include "server.hpp"
#include <string>

int main(int argc, char** argv) {
  int port = 7878;
  if (argc > 1) {
    port = std::stoi(argv[1]);
  }

  Server* server = new Server(port);
  server->service();

  return 0;
}
