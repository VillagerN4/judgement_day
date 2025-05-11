#include "DialogBox.hpp"

DialogBox::DialogBox(sf::Font& font, const sf::Vector2u& windowSize) {
    background.setSize(sf::Vector2f(windowSize.x - 40, 120));
    background.setPosition(20, windowSize.y - 140);
    background.setFillColor(sf::Color(0, 0, 0, 180));
    background.setOutlineThickness(2);
    background.setOutlineColor(sf::Color::White);

    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    text.setPosition(30, windowSize.y - 130);
    currentIndex = 0;
}

void DialogBox::setDialog(const std::vector<std::string>& lines) {
    dialogLines = lines;
    currentIndex = 0;
    updateText();
}

void DialogBox::nextLine() {
    if (currentIndex + 1 < dialogLines.size()) {
        currentIndex++;
        updateText();
    }
}

void DialogBox::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(text);
}

void DialogBox::updateText() {
    if (!dialogLines.empty()) {
        text.setString(dialogLines[currentIndex]);
    }
}
