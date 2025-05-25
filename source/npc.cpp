#include "npc.hpp"

NPC::NPC(sf::Vector2f position, sf::Vector2f size) {
    shape.setSize(size);
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(position);
}

void NPC::checkCollision(const sf::RectangleShape& player) {
    if (shape.getGlobalBounds().intersects(player.getGlobalBounds())) {
        shape.setFillColor(sf::Color::Red);
    } else {
        shape.setFillColor(sf::Color::Green);
    }
}

void NPC::draw(sf::RenderWindow& window) {
    window.draw(shape);
}
