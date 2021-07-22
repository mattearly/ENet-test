//http://enet.bespin.org/Tutorial.html

#include <enet/enet.h>
#include <stdexcept>
#include <cstdio>
#include "server.h"
#include "client.h"

int main()
{
  if(enet_initialize() != 0) { throw std::runtime_error("error init enet"); }

  Server server{};

  Client client{};

  client.connect();

  while (client.update()) {
    server.update();
  }

  client.cleanup();
  server.cleanup();
  atexit(enet_deinitialize);
  return 0;
}