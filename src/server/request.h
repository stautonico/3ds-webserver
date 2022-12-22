#pragma once

#include <map>
#include <string>

namespace HTTP {
enum Method {
  CONNECT,
  DELETE,
  GET,
  HEAD,
  OPTIONS,
  POST,
  PUT,
  TRACE,
};

class Status {
public:
  Status(int num, std::string description);

  int num() { return m_status_num; };
  std::string description() { return m_description; };

private:
  int m_status_num{};
  std::string m_description;
  std::string m_class;
};

static std::map<int, Status> statuses = {
    // 1XX
    {100, Status(100, std::string("Continue"))},
    {101, Status(101, std::string("Switching Protocols"))},
    {102, Status(102, std::string("Processing"))}, // Not supported (WebDEV)
    {103, Status(103, std::string("Early Hints"))},
    // 2XX
    {200, Status(200, std::string("OK"))},
    {201, Status(201, std::string("Created"))},
    {202, Status(202, std::string("Accepted"))},
    {203, Status(203, std::string("Non-Authoritative Information"))},
    {204, Status(204, std::string("No Content"))},
    {205, Status(205, std::string("Reset Content"))},
    {206, Status(206, std::string("Partial Content"))},
    {207, Status(207, std::string("Multi-Status"))}, // Not supported (WebDAV)
    {208, Status(208, std::string("Already Reported"))}, // Not supported (WebDAV)
    {226, Status(226, std::string("IM Used"))}, // Not supported (HTTP Delta Encoding)
    // 3XX
    {300, Status(300, std::string("Multiple Choices"))},
    {301, Status(301, std::string("Moved Permanently"))},
    {302, Status(302, std::string("Found"))},
    {303, Status(303, std::string("See Other"))},
    {304, Status(304, std::string("Not Modified"))},
    {305, Status(305, std::string("Use Proxy"))},
    {306, Status(306, std::string("unused"))},
    {307, Status(307, std::string("Temporary Redirect"))},
    {308, Status(308, std::string("Permanent Redirect"))},
    // 4XX
    {400, Status(400, std::string("Bad Request"))},
    {401, Status(401, std::string("Unauthorized"))},
    {402, Status(402, std::string("Payment Required"))},
    {403, Status(403, std::string("Forbidden"))},
    {404, Status(404, std::string("Not Found"))},
    {405, Status(405, std::string("Method Not Allowed"))},
    {406, Status(406, std::string("Not Acceptable"))},
    {407, Status(407, std::string("Proxy Authentication Required"))},
    {408, Status(408, std::string("Request Timeout"))},
    {409, Status(409, std::string("Conflict"))},
    {410, Status(410, std::string("Gone"))},
    {411, Status(411, std::string("Length Required"))},
    {412, Status(412, std::string("Precondition Failed"))},
    {413, Status(413, std::string("Payload Too Large"))},
    {414, Status(414, std::string("URI Too Long"))},
    {415, Status(415, std::string("Unsupported Media Type"))},
    {416, Status(416, std::string("Range Not Satisfiable"))},
    {417, Status(417, std::string("Expectation Failed"))},
    {418, Status(418, std::string("I'm a teapot"))},




};


class Request {
public:
  Request();
  ~Request();

private:
  Method m_method;
  std::map<std::string, std::string> m_headers;
  Status m_status;
};
} // namespace HTTP