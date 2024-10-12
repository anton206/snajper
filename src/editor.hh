#pragma once
#include "buffer.hh"
#include "fs.hh"
#include <SFML/Graphics.hpp>
#include <cassert>

enum class Mode { Edit, Command };

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

    if (m_mode == Mode::Edit) {
      m_cursor_rect.setPosition(static_cast<float>(m_cursor.x) * 10 + 5,
                                static_cast<float>(m_cursor.y) * 24 + 10);
      m_window.draw(m_cursor_rect);
    } else if (m_mode == Mode::Command) {
      m_text.setFillColor(sf::Color(0xb16286ff));
      m_text.setPosition(5, static_cast<float>(m_window.getSize().y) - 20);
      m_text.setString("M-x: ");
      m_window.draw(m_text);

      m_text.setFillColor(sf::Color(0xebdbb2ff));
      m_text.setPosition(50, static_cast<float>(m_window.getSize().y) - 20);
      m_text.setString(m_minibuffer);
      m_window.draw(m_text);
    }

    m_window.display();
  }

  void process_text_entered(sf::String s) {
    switch (s[0]) {
    case 8:   // Backspace
    case 13:  // Enter
    case 24:  // Ctrl+X
    case 27:  // Escape
    case 127: // Delete
      break;
    case 9: // Tab
      if (m_mode == Mode::Edit) {
        m_buffer.lines[m_cursor.y].insert(m_cursor.x,
                                          std::string(TAB_SIZE, ' '));
        m_cursor.x += TAB_SIZE;
      }
      break;
    default:
      if (m_mode == Mode::Edit) {
        m_buffer.lines[m_cursor.y].insert(m_cursor.x, s);
        m_cursor.x++;
      } else {
        m_minibuffer += s;
      }
    }
  }

  void process_key_pressed(sf::Event event) {
    if (m_mode != Mode::Edit) {
      if (event.key.code == sf::Keyboard::Escape ||
          event.key.code == sf::Keyboard::Enter) {
        m_mode = Mode::Edit;
      }
      return;
    }

    if (event.key.code == sf::Keyboard::Enter) {
      if (m_buffer.lines[m_cursor.y].getSize() - 1 < m_cursor.x) {
        m_buffer.lines.insert(m_buffer.lines.begin() + m_cursor.y + 1, "");
      } else {
        m_buffer.lines.insert(m_buffer.lines.begin() + m_cursor.y, "");
      }
      m_cursor.y++;
      m_cursor.x = 0;
    } else if (event.key.control && event.key.code == sf::Keyboard::X) {
      m_mode = Mode::Command;
      m_minibuffer = "";
    } else if (event.key.code == sf::Keyboard::Left) {
      if (m_cursor.x == 0) {
        if (m_cursor.y != 0) {
          m_cursor.y--;
          m_cursor.x = m_buffer.lines[m_cursor.y].getSize();
        }
      } else {
        m_cursor.x--;
      }
    } else if (event.key.code == sf::Keyboard::Right) {
      if (m_buffer.lines[m_cursor.y].getSize() - 1 < m_cursor.x) {
        if (m_cursor.y < m_buffer.lines.size()) {
          m_cursor.y++;
          m_cursor.x = 0;
        }
      } else {
        m_cursor.x++;
      }
    } else if (event.key.code == sf::Keyboard::Up) {
      if (m_cursor.y > 0) {
        m_cursor.y--;
        if (m_buffer.lines[m_cursor.y].getSize() < m_cursor.x) {
          m_cursor.x = m_buffer.lines[m_cursor.y].getSize();
        }
      }
    } else if (event.key.code == sf::Keyboard::Down) {
      if (m_cursor.y + 1 < m_buffer.lines.size()) {
        m_cursor.y++;
        if (m_buffer.lines[m_cursor.y].getSize() < m_cursor.x) {
          m_cursor.x = m_buffer.lines[m_cursor.y].getSize();
        }
      }
    } else if (event.key.code == sf::Keyboard::Home) {
      m_cursor.x = 0;
    } else if (event.key.code == sf::Keyboard::End) {
      m_cursor.x = m_buffer.lines[m_cursor.y].getSize();
    } else if (event.key.code == sf::Keyboard::Backspace) {
      if (m_cursor.y > 0 && m_buffer.lines[m_cursor.y].isEmpty()) {
        m_buffer.lines.erase(m_buffer.lines.begin() + m_cursor.y);
        m_cursor.y--;
        m_cursor.x = m_buffer.lines[m_cursor.y].getSize();
      } else if (m_cursor.x > 0) {
        m_cursor.x--;
        m_buffer.lines[m_cursor.y].erase(m_cursor.x);
      }
    }
  }

private:
  static const int TAB_SIZE = 2;

  sf::RenderWindow &m_window; // NOLINT
  sf::Font m_font;
  sf::Text m_text;
  sf::RectangleShape m_cursor_rect;

  Mode m_mode{Mode::Edit};
  Buffer m_buffer{};
  sf::String m_minibuffer;
  sf::Vector2u m_cursor;

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