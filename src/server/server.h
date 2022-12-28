#pragma once

#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

namespace HTTP {
class Server {
public:
  Server();
  ~Server();

  bool tick();

  bool failure() { return m_failure; }
  std::string error() { return m_error; };

  void stop() { m_running = false; };

private:
  bool m_failure{};
  int m_sockfd{};
  bool m_running = false;
  std::string m_error{};

  sockaddr_in m_sockaddr;
};
} // namespace HTTP