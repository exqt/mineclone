#include "application.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  // argv[1] = Host + Port (e.g. "localhost:7878")
  // argv[2] = name

  if (argc < 3) {
    // std::cout << "Usage: " << argv[0] << " <host:port> <name>" << std::endl;
    // return 1;
  }

  std::string host = argc == 1 ? "localhost" : argv[1];
  std::string name = argc == 1 ? "User" : argv[2];
  int port;

  // split host and port
  auto colonPos = host.find(':');
  if (colonPos == std::string::npos) {
    port = 7878;
  }
  else {
    port = std::stoi(host.substr(colonPos + 1));
    host = host.substr(0, colonPos);
  }

  Application app;
  app.init();
  app.connect(host, port, name);
  app.run();
  app.clean();

  return 0;
}
