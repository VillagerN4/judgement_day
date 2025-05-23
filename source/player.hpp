#pragma once

#include <SFML/Graphics.hpp>

class Player {
public:
    Player();
    void handleInput();
    void draw(sf::RenderWindow& window);

private:
    sf::RectangleShape shape;
};
