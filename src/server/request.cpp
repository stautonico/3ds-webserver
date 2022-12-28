#include "request.h"
#include "../util/string.h"

#include <iomanip>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

HTTP::Status::Status(int num, std::string description) {
  m_status_num = num;
  m_description = description;
  if (100 <= num && num <= 199) {
    m_class = "Informational";
  } else if (200 <= num && num <= 299) {
    m_class = "Successful";
  } else if (300 <= num && num <= 399) {
    m_class = "Redirection";
  } else if (400 <= num && num <= 499) {
    m_class = "Client error";
  } else if (500 <= num && num <= 599) {
    m_class = "Server error";
  } else {
    m_class = "Unknown";
  }
}

HTTP::Request::Request() { add_header("Server", "WebServe3DS"); }

bool HTTP::Request::add_header(std::string key, std::string value) {
  if (m_headers.find(key) == m_headers.end()) {
    m_headers[key] = value;
    return true;
  }

  return false;
}

bool HTTP::Request::remove_header(std::string key) {
  if (m_headers.find(key) != m_headers.end()) {
    m_headers.erase(m_headers.find(key));
    return true;
  }

  return false;
}

std::string HTTP::Request::get_header(std::string key) {
  if (m_headers.find(key) == m_headers.end()) {
    return std::string();
  }
  return m_headers[key];
}

bool HTTP::Request::add_query_param(std::string key, std::string value) {
  if (m_query.find(key) == m_query.end()) {
    m_query[key] = value;
    return true;
  }

  return false;
}

bool HTTP::Request::remove_query_param(std::string key) {
  if (m_query.find(key) != m_query.end()) {
    m_query.erase(m_query.find(key));
    return true;
  }

  return false;
}

HTTP::Request HTTP::Request::from_raw(std::string raw_request) {
  // TODO: Parse raw http requests
  auto lines = split(raw_request, '\n');

  // We expect exactly 3 arguments in the first line
  auto line = split(lines[0], ' ');

  if (line.size() != 3) {
    throw std::runtime_error("Invalid HTTP request");
  }

  //  printf("0: %s, 1: %s, 2: %s\n", line.at(0).c_str(), line.at(1).c_str(),
  //         line.at(2).c_str());

  Request r;
  r.set_method(method_from_string(line.at(0)));
  r.set_path(line.at(1));

  auto split_http_version = split(line.at(2), '/');
  if (split_http_version.size() != 2 || split_http_version.at(0) != "HTTP") {
    throw std::runtime_error("Invalid HTTP request");
  }

  r.set_version(std::stof(split_http_version.at(1)));

  return r;
}

std::string HTTP::Request::to_raw() {
  std::ostringstream ss;

  ss << std::fixed << std::setprecision(1);
  ss << "HTTP/" << m_version << " " << m_status.num() << " "
     << m_status.description() << "\r\n";
  for (auto const &x : m_headers) {
    ss << x.first << ": " << x.second << "\r\n";
  }

  if (!m_body.empty()) {
    ss << "\r\n" << m_body << "\r\n";
  }

  return ss.str();
}

std::string HTTP::Request::method() { return method_to_string(m_method); }