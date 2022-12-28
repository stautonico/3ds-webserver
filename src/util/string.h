#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

inline std::vector<std::string> split(std::string const &input,
                                      const char delim) {
  std::vector<std::string> out;
  std::string token;
  std::stringstream ss(input);

  while (getline(ss, token, delim)) {
    out.push_back(token);
  }

  return out;
}