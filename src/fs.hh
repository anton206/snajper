#pragma once

#include "buffer.hh"
#include <cassert>
#include <cstring>
#include <fstream>
#include <string>

namespace FS {

inline Buffer read_file(const std::string &path) {
  std::vector<sf::String> lines;

  std::ifstream file(path);
  assert(file.is_open());

  std::string line;
  while (std::getline(file, line)) {
    lines.emplace_back(line);
  }

  return {.name = basename(path.c_str()), .path = path, .lines = lines};
}

} // namespace FS