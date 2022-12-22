#include "request.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <stdexcept>

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

HTTP::Request::Request(Method method, std::string host, std::string path)
    : m_method(method), m_host(host), m_path(path) {}

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
  std::vector<std::string> lines;
  std::string token;
  std::stringstream ss(raw_request);

  while (getline(ss, token, '\n')) {
    lines.push_back(token);
  }

//  if (lines.size() < 2) {
//    throw std::exception("Invalid HTTP request");
//  }


  // We expect exactly 3 arguments in the first line
  std::vector<std::string> line;
  std::stringstream line_ss(lines[0]);

  while (getline(line_ss, token, ' ')) {
    line.push_back(token);
  }

  printf("Method: %s, path: %s, version: %s\n", line.at(0).c_str(), line.at(1).c_str(), line.at(2).c_str());

  Request r(POST, "", "");
  return r;
}