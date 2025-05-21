#include "Player.hpp"
#include <SFML/Window/Keyboard.hpp>

Player::Player() {
    shape.setSize(sf::Vector2f(50.f, 50.f));
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(sf::Vector2f(100.f, 100.f)); // <- poprawka
}

void Player::handleInput() {
    const float speed = 1.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W)) {
        shape.move(sf::Vector2f(0.f, -speed));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) {
        shape.move(sf::Vector2f(0.f, speed));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) {
        shape.move(sf::Vector2f(-speed, 0.f));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) {
        shape.move(sf::Vector2f(speed, 0.f));
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape);
}
