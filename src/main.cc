#include "buffer.hh"
#include "fs.hh"
#include <SFML/Graphics.hpp>

const int TAB_SIZE = 2;

void draw_buffer_text(sf::RenderWindow &window, sf::Text text,
                      const std::vector<sf::String> &lines) {
  sf::RenderTexture texture;
  texture.create(window.getSize().x, window.getSize().y);
  texture.clear(sf::Color::Transparent);

  for (size_t y = 0; y < lines.size(); y++) {
    for (size_t x = 0; x < lines[y].getSize(); x++) {
      text.setPosition(static_cast<float>(5 + 10 * x),
                       static_cast<float>(10 + y * 24));
      text.setString(lines[y][x]);
      texture.draw(text, sf::BlendMode(sf::BlendMode::One, sf::BlendMode::One,
                                       sf::BlendMode::Add));
    }
  }

  texture.display();
  window.draw(sf::Sprite(texture.getTexture()));
}

int main() {
  sf::RenderWindow window{{800, 800}, "Snajper"};
  window.setFramerateLimit(100);

  sf::Font font;
  assert(font.loadFromFile("JetBrainsMono-Regular.ttf"));

  Buffer buffer = FS::read_file("src/main.cc");
  window.setTitle(buffer.name + " - Snajper");
  sf::Vector2u cursor_pos;

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

        switch (s[0]) {
        case 8:  // Enter
        case 13: // Backspace
          break;
        case 9: // Tab
          buffer.lines[cursor_pos.y].insert(cursor_pos.x,
                                            std::string(TAB_SIZE, ' '));
          cursor_pos.x += TAB_SIZE;
          break;
        default:
          buffer.lines[cursor_pos.y].insert(cursor_pos.x, s);
          cursor_pos.x++;
        }
      } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter) {
          if (buffer.lines[cursor_pos.y].getSize() - 1 < cursor_pos.x) {
            buffer.lines.insert(buffer.lines.begin() + cursor_pos.y + 1, "");
          } else {
            buffer.lines.insert(buffer.lines.begin() + cursor_pos.y, "");
          }
          cursor_pos.y++;
          cursor_pos.x = 0;
        } else if (event.key.code == sf::Keyboard::Left) {
          if (cursor_pos.x == 0) {
            if (cursor_pos.y != 0) {
              cursor_pos.y--;
              cursor_pos.x = buffer.lines[cursor_pos.y].getSize();
            }
          } else {
            cursor_pos.x--;
          }
        } else if (event.key.code == sf::Keyboard::Right) {
          if (buffer.lines[cursor_pos.y].getSize() - 1 < cursor_pos.x) {
            if (cursor_pos.y < buffer.lines.size()) {
              cursor_pos.y++;
              cursor_pos.x = 0;
            }
          } else {
            cursor_pos.x++;
          }
        } else if (event.key.code == sf::Keyboard::Up) {
          if (cursor_pos.y > 0) {
            cursor_pos.y--;
            if (buffer.lines[cursor_pos.y].getSize() < cursor_pos.x) {
              cursor_pos.x = buffer.lines[cursor_pos.y].getSize();
            }
          }
        } else if (event.key.code == sf::Keyboard::Down) {
          if (cursor_pos.y + 1 < buffer.lines.size()) {
            cursor_pos.y++;
            if (buffer.lines[cursor_pos.y].getSize() < cursor_pos.x) {
              cursor_pos.x = buffer.lines[cursor_pos.y].getSize();
            }
          }
        } else if (event.key.code == sf::Keyboard::Home) {
          cursor_pos.x = 0;
        } else if (event.key.code == sf::Keyboard::End) {
          cursor_pos.x = buffer.lines[cursor_pos.y].getSize();
        } else if (event.key.code == sf::Keyboard::Backspace) {
          if (cursor_pos.y > 0 && buffer.lines[cursor_pos.y].isEmpty()) {
            buffer.lines.erase(buffer.lines.begin() + cursor_pos.y);
            cursor_pos.y--;
            cursor_pos.x = buffer.lines[cursor_pos.y].getSize();
          } else if (cursor_pos.x > 0) {
            cursor_pos.x--;
            buffer.lines[cursor_pos.y].erase(cursor_pos.x);
          }
        }
      } else if (event.type == sf::Event::Resized) {
        sf::FloatRect visible_area(0, 0, static_cast<float>(event.size.width),
                                   static_cast<float>(event.size.height));
        window.setView(sf::View(visible_area));
      }
    }

    window.clear(sf::Color(0x1d2021ff));

    draw_buffer_text(window, text, buffer.lines);

    cursor_rect.setPosition(static_cast<float>(cursor_pos.x) * 10 + 5,
                            static_cast<float>(cursor_pos.y) * 24 + 10);
    window.draw(cursor_rect);

    window.display();
  }
}