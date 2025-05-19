#pragma once
#include <SFML/Graphics.hpp>

class EndScreen {
public:
    EndScreen(sf::RenderWindow& window);
    void show(bool playerWon);
private:
    sf::RenderWindow& m_window;
    sf::Font m_font;
    sf::Text m_message;

    void setupMessage(bool playerWon);
};
