#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class DialogBox {
public:
    DialogBox(sf::Font& font, const sf::Vector2u& windowSize);
    void setDialog(const std::vector<std::string>& lines);
    void nextLine();
    void draw(sf::RenderWindow& window);

private:
    sf::RectangleShape background;
    sf::Text text;
    std::vector<std::string> dialogLines;
    size_t currentIndex;

    void updateText();
};
