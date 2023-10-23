#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "server.hpp"
#include "game.hpp"

int main(int argc, char** argv) {
  int port = 7878;
  if (argc > 1) {
    port = std::stoi(argv[1]);
  }

  Server* server = new Server(port);
  Game* game = new Game(server);

  server->onConnectCallback = [&](User* user) { game->onConnect(user); };
  server->onDisconnectCallback = [&](User* user) { game->onDisconnect(user); };

  std::cout << "Listening on port " << port << std::endl;

  bool running = true;
  while (running) {
    auto start = std::chrono::high_resolution_clock::now();

    // pop and handle network events
    server->service([&](User* user, DataReadStream& stream) {
      game->onRPC(user, stream);
    });

    // update game logic
    game->update();

    auto end = std::chrono::high_resolution_clock::now();

    // sleep for 1/60th of a second
    std::chrono::duration<double> elapsed = end - start;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60) - elapsed);
  }

  delete server;
  delete game;

  return 0;
}
