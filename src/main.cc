#include "editor.hh"
#include <SFML/Graphics.hpp>

int main() {
  sf::RenderWindow window{{800, 800}, "Snajper"};
  window.setFramerateLimit(100);

  Editor editor(window);
  editor.open_file("src/main.cc");

  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::TextEntered) {
        editor.process_text_entered(sf::String(event.text.unicode));
      } else if (event.type == sf::Event::KeyPressed) {
        editor.process_key_pressed(event);
      } else if (event.type == sf::Event::Resized) {
        sf::FloatRect visible_area(0, 0, static_cast<float>(event.size.width),
                                   static_cast<float>(event.size.height));
        window.setView(sf::View(visible_area));
      }
    }

    editor.draw();
  }
}