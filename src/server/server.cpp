#include "server.h"

#include "request.h"

#include <arpa/inet.h>
#include <ctime>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <malloc.h>
#include <netinet/in.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>

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

  while (true) {
    int connection = accept(m_sockfd, (struct sockaddr *)&client, &clientlen);
    if (connection < 0) {
      perror("Failed to init connection");
      return false;
    }

    printf("Connecting port %d from %s\n", client.sin_port,
           inet_ntoa(client.sin_addr));
    fcntl(connection, F_SETFL, fcntl(connection, F_GETFL, 0) & ~O_NONBLOCK);

    char buf[1024];
    memset(&buf, 0, 1024);
    auto bytesRead = read(connection, buf, 1024);

    try {
      Request r;

      r = Request::from_raw(std::string(buf));

      if (r.path() == "/exit") {
        break;
      }

      if (r.path() == "/") {
        r.set_path("/index.html");
      }

      std::filesystem::path f{"/tmp/webfiles" + r.path()};
      if (!std::filesystem::exists(f)) {
        std::string message = PageNotFoundResponse.to_raw();
        send(connection, message.c_str(), message.size(), 0);
      }

      struct stat result;
      stat(("/tmp/webfiles" + r.path()).c_str(), &result);
      auto mod_time = result.st_mtime;

      auto modified_header = r.get_header("If-Modified-Since");
      if (!modified_header.empty()) {
        printf("We're checking dates!\n");
        struct tm t = {0};
        if (strptime(modified_header.c_str(), "%a, %d %b %G %T %Z", &t) ==
            NULL) {
          printf("Parse time failed, ignoring cache");
        } else {
          time_t if_modified_time = mktime(&t);
          if (if_modified_time > mod_time) {
            auto message = NotModified.to_raw();
            send(connection, message.c_str(), message.size(), 0);
          }
        }
      } else {
        std::ifstream file("/tmp/webfiles" + r.path());
        if (!file.is_open()) {
        } else {
          file.seekg(0, std::ios_base::end); // Seek to end of file.
          const unsigned int file_length = file.tellg();
          file.seekg(0);
          std::vector<char> file_data(file_length);
          file.read(&file_data[0], file_length);

          Request res;
          res.set_status(HTTP::Statuses[200]);
          res.set_body(std::string(file_data.begin(), file_data.end()));


          std::tm* t = std::gmtime(&mod_time);
          std::stringstream ss;
          ss << std::put_time(t, "%a, %d %b %G %T %Z");
          res.add_header("Last-Modified", ss.str());

          std::stringstream content_length_str;
          content_length_str << file_length;
          res.add_header("Content-Length", content_length_str.str());

          time_t  current_time;
          time(&current_time);
          std::stringstream current_datetime;
          t = std::gmtime(&current_time);
          current_datetime << std::put_time(t, "%a, %d %b %G %T %Z");
          res.add_header("Date", current_datetime.str());

          std::string final_res = res.to_raw();

          send(connection, final_res.c_str(), final_res.size(), 0);
          file.close();
        }
      }
    } catch (const std::runtime_error &e) {
      std::string message = InternalServerErrorResponse.to_raw();
      send(connection, message.c_str(), message.size(), 0);
    }
    close(connection);
  }

  return true;
}