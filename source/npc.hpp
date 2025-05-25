#pragma once
#include <SFML/Graphics.hpp>

class NPC {
public:
    NPC(sf::Vector2f position, sf::Vector2f size);
    void checkCollision(const sf::RectangleShape& player);
    void draw(sf::RenderWindow& window);

private:
    sf::RectangleShape shape;
};
