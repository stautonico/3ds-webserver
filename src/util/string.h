#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

inline std::vector<std::string> split(std::string const &input, const char delim) {
  std::vector<std::string> out;
  std::string token;
  std::stringstream ss(input);

  while (getline(ss, token, delim)) {
    out.push_back(token);
  }

  return out;
}

//void tokenize(std::string const &str, const char delim,
//   std::vector<std::string> &out) {
//  size_t start;
//  size_t end = 0;
//
//  while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
//    end = str.find(delim, start);
//    out.push_back(str.substr(start, end - start));
//  }
//}