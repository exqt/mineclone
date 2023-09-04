extern "C" {
  #include <enet/enet.h>
}

#include "server.hpp"

int main() {
  Server server;
  server.service();

  return 0;
}
