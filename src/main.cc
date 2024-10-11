#include <SFML/Graphics.hpp>

int main() {
  auto window = sf::RenderWindow{{800, 800}, "Snajper"};
  window.setFramerateLimit(200);

  while (window.isOpen()) {
    for (auto event = sf::Event{}; window.pollEvent(event);) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    window.clear();
    window.display();
  }
}