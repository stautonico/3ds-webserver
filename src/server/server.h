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

private:
  bool m_failure{};
  int m_sockfd{};

  sockaddr_in m_sockaddr;
};
} // namespace HTTP