#include "server.h"

#include "../util/string.h"
#include "request.h"
#include "tls.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <malloc.h>
#include <netinet/in.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
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
    m_failure = true;
    m_error = "Memalign failed to allocate";
  }

  // Now intialise soc:u service
  if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
    printf("socInit: 0x%08X\n", (unsigned int)ret);
    m_failure = true;
    m_error = "Failed to run socInit";
  }

  // register socShutdown to run at exit
  // atexit functions execute in reverse order so this runs before gfxExit
  atexit(socClose);
#endif

  m_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

  if (m_sockfd < 0) {
    perror("Socket creation failed");
    m_failure = true;
    m_error = "Socket creation failed";
    return;
  }

  const int opt = 1;

#ifdef SO_REUSEPORT
  if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    perror("Failed to set socket options (SO_REUSEPORT)");
    m_failure = true;
    m_error = "Failed to set socket options (SO_REUSEPORT)";
    return;
  }
#endif

  if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("Failed to set socket options (SO_REUSEADDR)");
    m_failure = true;
    m_error = "Failed to set socket options (SO_REUSEADDR)";
    return;
  }

  memset(&m_sockaddr, 0, sizeof(m_sockaddr));

  m_sockaddr.sin_family = AF_INET;
#if IS_3DS
  m_sockaddr.sin_addr.s_addr = gethostid();
#else
  m_sockaddr.sin_addr.s_addr = INADDR_ANY;
#endif
  m_sockaddr.sin_port =
      htons(8000); // TODO: Make this configurable through a config file

  printf("Trying to bind to %s:%d\n", inet_ntoa(m_sockaddr.sin_addr), 8000);

#if IS_3DS
  if (bind(m_sockfd, (struct sockaddr *)&m_sockaddr, sizeof(m_sockaddr))) {
#else
  if (bind(m_sockfd, (struct sockaddr *)&m_sockaddr, sizeof(m_sockaddr)) < 0) {
#endif
    perror("Socket bind failure");
    m_failure = true;
    m_error = "Failed to bind socket";
    return;
  }

#if IS_3DS
  fcntl(m_sockfd, F_SETFL, fcntl(m_sockfd, F_GETFL, 0) | O_NONBLOCK);
#endif

#if IS_3DS
  if (listen(m_sockfd, 10)) {
#else
  if (listen(m_sockfd, 10) < 0) {
#endif
    perror("Failed to listen on socket");
    m_failure = true;
    m_error = "Failed to listen";
    return;
  }

  m_failure = false;
}
HTTP::Server::~Server() { close(m_sockfd); }

bool HTTP::Server::tick() {
  struct sockaddr_in client;
  memset(&client, 0, sizeof(client));

  uint32_t clientlen = sizeof(client);

  int connection = accept(m_sockfd, (struct sockaddr *)&client, &clientlen);
  if (connection < 0) {
    return true;
  }

  fcntl(connection, F_SETFL, fcntl(connection, F_GETFL, 0) & ~O_NONBLOCK);

  char buf[1024];
  memset(&buf, 0, 1024);
  read(connection, buf, 1024);

  try {
    Request r;

    r = Request::from_raw(std::string(buf));

    if (r.path() == "/") {
      r.set_path("/index.html");
    }

    // Sanitize relative path (local file inclusion)
    // TODO: Make this more robust
    std::string path = r.path();
    std::string to_find = "..";

    while (true) {
      std::string::size_type i = path.find(to_find);

      if (i != std::string::npos)
        path.erase(i, to_find.length());
      else
        break;
    }

    r.set_path(path);

    std::filesystem::path f{"./3ds-webserver-files" + r.path()};
    auto normal_path = f.lexically_normal();
    if (!std::filesystem::exists(normal_path)) {
      printf("%s %s - %d\n", r.method().c_str(), r.path().c_str(),
             PageNotFoundResponse.status().num());
      std::string message = PageNotFoundResponse.to_raw();
      send(connection, message.c_str(), message.size(), 0);
    }

    if (std::filesystem::is_directory(f)) {
      normal_path.append("index.html");
      if (r.path().back() == '/')
        r.set_path(r.path() + "index.html");
      else
        r.set_path(r.path() + "/index.html");
    }

    struct stat result;
    stat(("./3ds-webserver-files" + r.path()).c_str(), &result);
    auto mod_time = result.st_mtime;

    auto modified_header = r.get_header("If-Modified-Since");
    // TODO: Enable later
    if (!modified_header.empty()) {
      printf("We're checking dates!\n");
      struct tm t = {0};
#if IS_LINUX
      // Check if cache
      if (strptime(modified_header.c_str(), "%a, %d %b %G %T %Z", &t) ==
          nullptr) {
        printf("Parse time failed, ignoring cache");
      } else {
        time_t if_modified_time = mktime(&t);
        if (if_modified_time > mod_time) {
          auto message = NotModified.to_raw();
          send(connection, message.c_str(), message.size(), 0);
        }
      }
#endif
    } else {
      std::ifstream file("./3ds-webserver-files" + r.path());
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

        std::tm *t = std::gmtime(&mod_time);
        std::stringstream ss;
        ss << std::put_time(t, "%a, %d %b %G %T %Z");
        res.add_header("Last-Modified", ss.str());

        std::stringstream content_length_str;
        content_length_str << file_length;
        res.add_header("Content-Length", content_length_str.str());

        time_t current_time;
        time(&current_time);
        std::stringstream current_datetime;
        t = std::gmtime(&current_time);
        current_datetime << std::put_time(t, "%a, %d %b %G %T %Z");
        res.add_header("Date", current_datetime.str());

        // Add the Content-type header
        auto ext = split(r.path(), '.').back();

        res.add_header("Content-Type", HTTP::ext_to_mime(ext));

        res.add_header("Cache-Control", "max-age=3600");

        printf("%s %s - %d\n", r.method().c_str(), r.path().c_str(),
               res.status().num());

        std::string final_res = res.to_raw();
        send(connection, final_res.c_str(), final_res.size(), 0);
        file.close();
        fcntl(m_sockfd, F_SETFL, fcntl(m_sockfd, F_GETFL, 0) | O_NONBLOCK);
      }
    }
  } catch (const std::runtime_error &e) {
    m_error = e.what();
    printf("%s\n", m_error.c_str());
    std::string message = InternalServerErrorResponse.to_raw();
    send(connection, message.c_str(), message.size(), 0);
  }
  close(connection);

  return true;
}
