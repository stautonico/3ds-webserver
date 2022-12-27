#pragma once

#include <algorithm>
#include <map>
#include <stdexcept>
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

inline Method method_from_string(std::string name) {
  transform(name.begin(), name.end(), name.begin(), ::toupper);
  if (name == "CONNECT")
    return CONNECT;
  else if (name == "DELETE")
    return DELETE;
  else if (name == "GET")
    return GET;
  else if (name == "HEAD")
    return HEAD;
  else if (name == "OPTIONS")
    return OPTIONS;
  else if (name == "POST")
    return POST;
  else if (name == "PUT")
    return PUT;
  else if (name == "TRACE")
    return TRACE;
  throw std::runtime_error("Invalid method");
}

inline std::string method_to_string(Method m) {
  if (m == CONNECT)
    return "CONNECT";
  else if (m == DELETE)
    return "DELETE";
  else if (m == GET)
    return "GET";
  else if (m == HEAD)
    return "HEAD";
  else if (m == OPTIONS)
    return "OPTIONS";
  else if (m == POST)
    return "POST";
  else if (m == PUT)
    return "PUT";
  else if (m == TRACE)
    return "TRACE";
  throw std::runtime_error("Invalid method");
}

class Status {
public:
  Status(){}; // Used for empty Statuses
  Status(int num, std::string description);

  int num() { return m_status_num; };
  std::string description() { return m_description; };

private:
  int m_status_num{};
  std::string m_description;
  std::string m_class;
};

static std::map<int, Status> Statuses = {
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
    {208,
     Status(208, std::string("Already Reported"))}, // Not supported (WebDAV)
    {226, Status(226, std::string(
                          "IM Used"))}, // Not supported (HTTP Delta Encoding)
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
    {421, Status(421, std::string("Misdirected Request"))},
    {422, Status(422, std::string(
                          "Unprocessable Entity"))}, // Not supported (WebDAV)
    {423, Status(423, std::string("Locked"))},       // Not supported (WebDAV)
    {424,
     Status(424, std::string("Failed Dependency"))}, // Not supported (WebDAV)
    {425, Status(425, std::string("Too Early"))},
    {426, Status(426, std::string("Upgrade Required"))},
    {428, Status(428, std::string("Precondition Required"))},
    {429, Status(429, std::string("Too Many Requests"))},
    {431, Status(431, std::string("Request Header Fields Too Large"))},
    {451, Status(451, std::string("Unavailable For Legal Reasons"))},
    // 5XX
    {500, Status(500, std::string("Internal Server Error"))},
    {501, Status(501, std::string("Not Implemented"))},
    {502, Status(502, std::string("Bad Gateway"))},
    {503, Status(503, std::string("Service Unavailable"))},
    {504, Status(504, std::string("Gateway Timeout"))},
    {505, Status(505, std::string("HTTP Version Not Supported"))},
    {506, Status(506, std::string("Variant Also Negotiates"))},
    {507, Status(507, std::string(
                          "Insufficient Storage"))}, // Not supported (WebDAV)
    {510, Status(510, std::string("Not Extended"))},
    {511, Status(511, std::string("Network Authentication Required"))},

};

class Request {
public:
  Request();
  ~Request(){};

  void set_method(Method method) { m_method = method; };
  void set_version(float version) { m_version = version; }
  void set_body(std::string body) { m_body = body; };
  void set_path(std::string path) { m_path = path; };
  void set_status(Status status) { m_status = status; };

  bool add_header(std::string key, std::string value);
  bool remove_header(std::string key);
  std::string get_header(std::string key);

  bool add_query_param(std::string key, std::string value);
  bool remove_query_param(std::string key);

  static Request from_raw(std::string raw_request);
  std::string to_raw();

  std::string path() { return m_path; };
  std::string method();
  Status status() {return m_status;};

private:
  Method m_method;
  std::string m_host;
  std::string m_path;
  float m_version{1.1};
  std::map<std::string, std::string> m_headers;
  std::map<std::string, std::string> m_query;
  std::string m_body;
  Status m_status;
};

static Request build_304_response() {
  Request r;
  r.set_status(Statuses[304]);

  return r;
}

static Request build_404_response() {
  Request r;
  r.set_status(Statuses[404]);
  r.set_body("<h1>404 Page not found :(</h1>");

  return r;
}

static Request build_500_response() {
  Request r;
  r.set_status(Statuses[500]);
  r.set_body("<h1>500 Internal server error occurred :(</h1>");

  return r;
}

static Request NotModified = build_304_response();
static Request PageNotFoundResponse = build_404_response();
static Request InternalServerErrorResponse = build_500_response();

} // namespace HTTP