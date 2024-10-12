#pragma once
#include "buffer.hh"
#include "fs.hh"
#include <SFML/Graphics.hpp>
#include <cassert>

class Editor {
public:
  explicit Editor(sf::RenderWindow &window) : m_window(window) {
    m_font = sf::Font();
    assert(m_font.loadFromFile("JetBrainsMono-Regular.ttf"));

    m_text = sf::Text("", m_font, 17);
    m_text.setFillColor(sf::Color(0xebdbb2ff));

    m_cursor_rect = sf::RectangleShape(sf::Vector2f(2, 20));
    m_cursor_rect.setFillColor(sf::Color(0xa89984ff));
  }

  void open_file(const std::string &path) {
    m_buffer = FS::read_file(path);
    m_window.setTitle(m_buffer.name + " - Snajper");
  }

  void draw() {
    m_window.clear(sf::Color(0x1d2021ff));

    draw_buffer_text(m_buffer.lines);

    m_cursor_rect.setPosition(static_cast<float>(cursor_pos.x) * 10 + 5,
                              static_cast<float>(cursor_pos.y) * 24 + 10);
    m_window.draw(m_cursor_rect);

    m_window.display();
  }

  void process_text_entered(sf::String s) {
    switch (s[0]) {
    case 8:  // Enter
    case 13: // Backspace
      break;
    case 9: // Tab
      m_buffer.lines[cursor_pos.y].insert(cursor_pos.x,
                                          std::string(TAB_SIZE, ' '));
      cursor_pos.x += TAB_SIZE;
      break;
    default:
      m_buffer.lines[cursor_pos.y].insert(cursor_pos.x, s);
      cursor_pos.x++;
    }
  }

  void process_key_pressed(sf::Event event) {
    if (event.key.code == sf::Keyboard::Enter) {
      if (m_buffer.lines[cursor_pos.y].getSize() - 1 < cursor_pos.x) {
        m_buffer.lines.insert(m_buffer.lines.begin() + cursor_pos.y + 1, "");
      } else {
        m_buffer.lines.insert(m_buffer.lines.begin() + cursor_pos.y, "");
      }
      cursor_pos.y++;
      cursor_pos.x = 0;
    } else if (event.key.code == sf::Keyboard::Left) {
      if (cursor_pos.x == 0) {
        if (cursor_pos.y != 0) {
          cursor_pos.y--;
          cursor_pos.x = m_buffer.lines[cursor_pos.y].getSize();
        }
      } else {
        cursor_pos.x--;
      }
    } else if (event.key.code == sf::Keyboard::Right) {
      if (m_buffer.lines[cursor_pos.y].getSize() - 1 < cursor_pos.x) {
        if (cursor_pos.y < m_buffer.lines.size()) {
          cursor_pos.y++;
          cursor_pos.x = 0;
        }
      } else {
        cursor_pos.x++;
      }
    } else if (event.key.code == sf::Keyboard::Up) {
      if (cursor_pos.y > 0) {
        cursor_pos.y--;
        if (m_buffer.lines[cursor_pos.y].getSize() < cursor_pos.x) {
          cursor_pos.x = m_buffer.lines[cursor_pos.y].getSize();
        }
      }
    } else if (event.key.code == sf::Keyboard::Down) {
      if (cursor_pos.y + 1 < m_buffer.lines.size()) {
        cursor_pos.y++;
        if (m_buffer.lines[cursor_pos.y].getSize() < cursor_pos.x) {
          cursor_pos.x = m_buffer.lines[cursor_pos.y].getSize();
        }
      }
    } else if (event.key.code == sf::Keyboard::Home) {
      cursor_pos.x = 0;
    } else if (event.key.code == sf::Keyboard::End) {
      cursor_pos.x = m_buffer.lines[cursor_pos.y].getSize();
    } else if (event.key.code == sf::Keyboard::Backspace) {
      if (cursor_pos.y > 0 && m_buffer.lines[cursor_pos.y].isEmpty()) {
        m_buffer.lines.erase(m_buffer.lines.begin() + cursor_pos.y);
        cursor_pos.y--;
        cursor_pos.x = m_buffer.lines[cursor_pos.y].getSize();
      } else if (cursor_pos.x > 0) {
        cursor_pos.x--;
        m_buffer.lines[cursor_pos.y].erase(cursor_pos.x);
      }
    }
  }

private:
  static const int TAB_SIZE = 2;

  sf::RenderWindow &m_window; // NOLINT
  sf::Font m_font;
  sf::Text m_text;
  sf::RectangleShape m_cursor_rect;

  Buffer m_buffer{};
  sf::Vector2u cursor_pos;

  void draw_buffer_text(const std::vector<sf::String> &lines) {
    sf::RenderTexture texture;
    texture.create(m_window.getSize().x, m_window.getSize().y);
    texture.clear(sf::Color::Transparent);

    for (size_t y = 0; y < lines.size(); y++) {
      for (size_t x = 0; x < lines[y].getSize(); x++) {
        m_text.setPosition(static_cast<float>(5 + 10 * x),
                           static_cast<float>(10 + y * 24));
        m_text.setString(lines[y][x]);
        texture.draw(m_text,
                     sf::BlendMode(sf::BlendMode::One, sf::BlendMode::One,
                                   sf::BlendMode::Add));
      }
    }

    texture.display();
    m_window.draw(sf::Sprite(texture.getTexture()));
  }
};