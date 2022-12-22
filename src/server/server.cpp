#include "server.h"

#include "request.h"

#include <arpa/inet.h>
#include <malloc.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdexcept>


#if IS_3DS
#include <3ds.h>

void socClose() { socExit(); }
#endif

HTTP::Server::Server() {
#if IS_3DS
#define SOC_ALIGN 0x1000
#define SOC_BUFFERSIZE 0x100000
  int ret;
  u32 *SOC_buffer = NULL;
  SOC_buffer = (u32 *)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

  if (SOC_buffer == NULL) {
    printf("memalign: failed to allocate\n");
  }

  // Now intialise soc:u service
  if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
    printf("socInit: 0x%08X\n", (unsigned int)ret);
  }

  // register socShutdown to run at exit
  // atexit functions execute in reverse order so this runs before gfxExit
  atexit(socClose);
#endif

  m_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

  if (m_sockfd < 0) {
    perror("Socket creation failed");
    m_failure = true;
    return;
  }

  const int opt = 1;

  if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("Failed to set socket options");
    m_failure = true;
    return;
  }

  m_sockaddr.sin_family = AF_INET;
  #if IS_3DS
  m_sockaddr.sin_addr.s_addr = gethostid();
  #else
  m_sockaddr.sin_addr.s_addr = INADDR_ANY;
  #endif
  m_sockaddr.sin_port =
      htons(8000); // TODO: Make this configurable through a config file

  printf("Trying to bind to %s:%d\n", inet_ntoa(m_sockaddr.sin_addr), 8000);

  if (bind(m_sockfd, (struct sockaddr *)&m_sockaddr, sizeof(m_sockaddr)) < 0) {
    perror("Socket bind failure");
    m_failure = true;
    return;
  }

  #if IS_3DS
  fcntl(m_sockfd, F_SETFL, fcntl(m_sockfd, F_GETFL, 0) | O_NONBLOCK);
  #endif

  m_failure = false;
}
HTTP::Server::~Server() { close(m_sockfd); }

bool HTTP::Server::run() {
  if (listen(m_sockfd, 10) < 0) {
    perror("Failed to listen on socket");
    return false;
  }

  struct sockaddr_in client;

  uint32_t clientlen = sizeof(client);

  auto addrlen = sizeof(sockaddr);
  int connection =
      accept(m_sockfd, (struct sockaddr *) &client, &clientlen);
  if (connection < 0) {
    perror("Failed to init connection");
    return false;
  }

  printf("Connecting port %d from %s\n", client.sin_port, inet_ntoa(client.sin_addr));
  fcntl(connection, F_SETFL, fcntl(connection, F_GETFL, 0) & ~O_NONBLOCK);

  char buf[1024];
  memset(&buf, 0, 1024);
  auto bytesRead = read(connection, buf, 1024);

  try {
    Request r = Request::from_raw(std::string(buf));
  } catch (const std::exception& e) {
    printf("We had an execption: %s\n", e.what());
  }

//  printf("Our read message was: '%s'\n", buf);

  send(connection, buf, bytesRead, 0);

  return true;
}