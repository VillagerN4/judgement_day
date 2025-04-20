#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

using namespace sf;
using namespace std;

enum class GameState { Menu, Settings, Credits, Game };

const float PLAYER_SIZE = 50.f;
const float MOVE_SPEED = 200.f;
const Vector2u WINDOW_SIZE(1920, 1200);

void handleInput(Vector2f& movement) {
    if (Keyboard::isKeyPressed(Keyboard::Key::W))
        movement.y -= MOVE_SPEED;
    if (Keyboard::isKeyPressed(Keyboard::Key::S))
        movement.y += MOVE_SPEED;
    if (Keyboard::isKeyPressed(Keyboard::Key::A))
        movement.x -= MOVE_SPEED;
    if (Keyboard::isKeyPressed(Keyboard::Key::D))
        movement.x += MOVE_SPEED;
}

void constrainToWindow(RectangleShape& player) {
    Vector2f pos = player.getPosition();
    if (pos.x < 0) pos.x = 0;
    if (pos.y < 0) pos.y = 0;
    if (pos.x + PLAYER_SIZE > WINDOW_SIZE.x) pos.x = WINDOW_SIZE.x - PLAYER_SIZE;
    if (pos.y + PLAYER_SIZE > WINDOW_SIZE.y) pos.y = WINDOW_SIZE.y - PLAYER_SIZE;
    player.setPosition(pos);
}

int main() {
    RenderWindow window(VideoMode(WINDOW_SIZE), "Judgement Day");
    window.setFramerateLimit(60);

    GameState state = GameState::Menu;

    Font font;
    if (!font.openFromFile("txtures/fonts/font1.otf")) {
        cout << "Font not available!" << endl;
        return 1;
    }

    Text menuItems[4] = {
        Text(font, "Start", 108),
        Text(font, "Settings", 108),
        Text(font, "Credits", 108),
        Text(font, "Exit", 108)
    };
    int selectedIndex = 0;
    Text title = Text(font,"Judgement Day", 108);
    title.setPosition(Vector2f(WINDOW_SIZE.x - 1300.f, 20.f));
    title.setFillColor(Color::Red);
    for (int i = 0; i < 4; ++i) {
        menuItems[i].setPosition(Vector2f(40.f, 500.f + i * 110));
        menuItems[i].setFillColor(i == selectedIndex ? Color::Red : Color::White);
    }
    RectangleShape player(Vector2f(PLAYER_SIZE, PLAYER_SIZE));
    player.setFillColor(Color::Blue);
    player.setPosition(Vector2f(375.f, 275.f));

    Texture texture;
    if (!texture.loadFromFile("txtures/Cos.png")) {
        cout << "Background not available!" << endl;
    }
    Sprite sprite(texture);

    Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }

            if (state == GameState::Menu) {
                if (event->is<Event::KeyPressed>()) {
                    auto keyEvent = event->getIf<Event::KeyPressed>();
                    if (keyEvent) {
                        if (keyEvent->code == Keyboard::Key::W) {
                            if(selectedIndex > 0){
                            menuItems[selectedIndex].setFillColor(Color::White);
                            selectedIndex --;
                            menuItems[selectedIndex].setFillColor(Color::Red);
                            }
                        }
                        else if (keyEvent->code == Keyboard::Key::S) {
                            if(selectedIndex < 3){
                            menuItems[selectedIndex].setFillColor(Color::White);
                            selectedIndex ++;
                            menuItems[selectedIndex].setFillColor(Color::Red);
                            }
                        }
                        else if (keyEvent->code == Keyboard::Key::Enter) {
                            if (selectedIndex == 0) {
                                state = GameState::Game;
                            }
                            else if (selectedIndex == 1) {
                                state = GameState::Settings;
                            }
                            else if (selectedIndex == 2) {
                                state = GameState::Credits;
                            }
                            else{
                                window.close();
                            }
                        }
                    }
                }
            }
        }

        window.clear();

        if (state == GameState::Menu) {
            //Dodam grafikę do tla tylko jak zrobię i muzykę tylko nagram z gitary
            window.draw(title);
            for (const auto& item : menuItems)
                window.draw(item);
        }
        else if (state == GameState::Game) {
            //Tutaj cała gra
            Vector2f movement(0.f, 0.f);
            handleInput(movement);
            player.move(movement * dt);
            constrainToWindow(player);

            window.draw(sprite);
            window.draw(player);
        }
        else if (state == GameState::Settings){
            //Zrobie ustawienia do muzyki itp - Adam Stec
        }
        else{
            //Credits to na końcu zrobię Adam Stec
        }

        window.display();
    }

    return 0;
}
