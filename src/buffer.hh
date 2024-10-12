#pragma once
#include <SFML/System/String.hpp>
#include <vector>

struct Buffer {
  std::string name;
  std::string path;
  std::vector<sf::String> lines{""};

  std::string to_string() {
    std::string out;
    for (const auto &line : lines) {
      out += line.toAnsiString();
    }
    return out;
  }
};