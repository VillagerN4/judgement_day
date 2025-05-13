#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <SFML/Graphics.hpp>
using namespace sf;
class Platform {
public:
    RectangleShape shape;

    Platform(Vector2f size, Vector2f position, Color color) {
        shape.setSize(size);
        shape.setFillColor(color);
        shape.setPosition(position);
    }
};

#endif
