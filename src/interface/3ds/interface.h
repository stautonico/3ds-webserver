#pragma once

#include "../../server/server.h"

class Interface {
public:
  Interface();
  ~Interface();

  void run();

private:
  HTTP::Server m_server;
};