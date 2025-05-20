// #include "EndScreen.hpp"
// #include <iostream>

// EndScreen::EndScreen(sf::RenderWindow& window) : m_window(window) {
//     if (!m_font.loadFromFile("assets/fonts/font1.otf")) {
//         std::cerr << "Nie można załadować czcionki!" << std::endl;
//     }
//     m_message.setFont(m_font);
//     m_message.setCharacterSize(48);
//     m_message.setStyle(sf::Text::Bold);
// }

// void EndScreen::setupMessage(bool playerWon) {
//     if (playerWon) {
//         m_message.setString("Zwycięstwo! Pokonałeś Bossa!");
//         m_message.setFillColor(sf::Color::Green);
//     } else {
//         m_message.setString("Przegrana! Boss cię pokonał...");
//         m_message.setFillColor(sf::Color::Red);
//     }

//     sf::FloatRect textRect = m_message.getLocalBounds();
//     m_message.setOrigin(textRect.left + textRect.width / 2.0f,
//                         textRect.top + textRect.height / 2.0f);
//     m_message.setPosition(m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f);
// }

// void EndScreen::show(bool playerWon) {
//     setupMessage(playerWon);

//     bool running = true;
//     while (running) {
//         sf::Event event;
//         while (m_window.pollEvent(event)) {
//             if (event.type == sf::Event::Closed ||
//                 event.type == sf::Event::KeyPressed ||
//                 event.type == sf::Event::MouseButtonPressed) {
//                 running = false;
//             }
//         }

//         m_window.clear(sf::Color::Black);
//         m_window.draw(m_message);
//         m_window.display();
//     }
// }

// /* Jak uzywac w main.cpp
// #include <SFML/Graphics.hpp>
// #include "EndScreen.hpp"

// int main() {
//     sf::RenderWindow window(sf::VideoMode(800, 600), "RPG Boss Battle");

//     EndScreen endScreen(window);
//     bool playerWon = true; // albo false, w zależności od wyniku walki
//     endScreen.show(playerWon);

//     return 0;
// }
// */


