#include <SFML/Graphics.hpp>
#include <cassert>

struct Buffer {
  std::vector<sf::String> lines{""};

  sf::String &last_line() { return lines[lines.size() - 1]; }
};

int main() {
  auto window = sf::RenderWindow{{800, 800}, "Snajper"};
  window.setFramerateLimit(200);

  sf::Font font;
  assert(font.loadFromFile("JetBrainsMono-Regular.ttf"));

  Buffer buffer;
  sf::Vector2f cursor_pos;

  sf::Text text("", font, 17);
  text.setFillColor(sf::Color(0xebdbb2ff));

  sf::RectangleShape cursor_rect(sf::Vector2f(2, 20));
  cursor_rect.setFillColor(sf::Color(0xa89984ff));

  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::TextEntered) {
        auto s = sf::String(event.text.unicode);

        if (s[0] == 13) {
          cursor_pos.x = 0;
          cursor_pos.y++;
          buffer.last_line() += "\n";
        } else if (s[0] == 8) {
          if (buffer.last_line().getSize() > 0) {
            buffer.last_line().erase(buffer.last_line().getSize() - 1);
            cursor_pos.x--;
          }
        } else {
          cursor_pos.x++;
          buffer.last_line() += s;
        }
      }
    }

    window.clear(sf::Color(0x1d2021ff));

    for (size_t i = 0; i < buffer.lines.size(); i++) {
      text.setPosition(10, static_cast<float>(10 + i * 20));
      text.setString(buffer.lines[i]);
      window.draw(text);
    }

    cursor_rect.setPosition(cursor_pos.x * 10 + 10, cursor_pos.y * 22 + 10);
    window.draw(cursor_rect);

    window.display();
  }
}