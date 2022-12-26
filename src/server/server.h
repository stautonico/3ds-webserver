#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace HTTP {
class Server {
public:
  Server();
  ~Server();

  bool run();

  bool failure() { return m_failure; }

  void stop() {m_running = false;};

private:
  bool m_failure{};
  int m_sockfd{};
  bool m_running = false;

  sockaddr_in m_sockaddr;
};
} // namespace HTTP