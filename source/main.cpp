#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <iostream>

using namespace sf;
using namespace std;

enum class GameState { Menu, Settings, Credits, Game, SaveSelect };

const float PLAYER_SIZE = 50.f;
const float MOVE_SPEED = 200.f;
const Vector2u WINDOW_SIZE(1920, 1200);

void SaveFile(int slot) {
    string filename = "saves/save" + to_string(slot + 1) + ".txt";
    ofstream file(filename);
    if (file.is_open()) {
        file.close();
        cout << "Save " << slot + 1 << " created!" << endl;
    } else {
        cerr << "Could not create save file!" << endl;
    }
}

// void handleInput(Vector2f& movement) {
//     if (Keyboard::isKeyPressed(Keyboard::Key::W))
//         movement.y -= MOVE_SPEED;
//     if (Keyboard::isKeyPressed(Keyboard::Key::S))
//         movement.y += MOVE_SPEED;
//     if (Keyboard::isKeyPressed(Keyboard::Key::A))
//         movement.x -= MOVE_SPEED;
//     if (Keyboard::isKeyPressed(Keyboard::Key::D))
//         movement.x += MOVE_SPEED;
// }

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
    Text title(font, "Judgement Day", 108);
    title.setPosition(Vector2f(WINDOW_SIZE.x - 1300.f, 20.f));
    title.setFillColor(Color::Red);

    for (int i = 0; i < 4; ++i) {
        menuItems[i].setPosition(Vector2f(40.f, 500.f + i * 110));
        menuItems[i].setFillColor(i == selectedIndex ? Color::Red : Color::White);
    }

    RectangleShape player(Vector2f(PLAYER_SIZE, PLAYER_SIZE));
    player.setFillColor(Color::Blue);
    player.setPosition(Vector2f(375.f, 275.f));

    float musicVolume = 50.f;
    bool isFullscreen = false;
    int settingsIndex = 0;

    Music music;
    if (music.openFromFile("audio/music1.ogg")) {
        music.setLooping(true);
        music.setVolume(musicVolume);
        music.play();
    } else {
        cout << "Music not available!" << endl;
    }

    Texture texture;
    if (!texture.loadFromFile("txtures/Cos.png")) {
        cout << "Background not available!" << endl;
    }
    Sprite sprite(texture);

    int saveSelectedIndex = 0;
    Text saveSlots[3] = {
        Text(font, "Save 1", 100),
        Text(font, "Save 2", 100),
        Text(font, "Save 3", 100)
    };
    for (int i = 0; i < 3; ++i) {
        saveSlots[i].setPosition(Vector2f(100.f, 400.f + i * 150.f));
        saveSlots[i].setFillColor(i == saveSelectedIndex ? Color::Red : Color::White);
    }

    Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                window.close();

            if (state == GameState::Menu) {
                if (event->is<Event::KeyPressed>()) {
                    auto keyEvent = event->getIf<Event::KeyPressed>();
                    if (keyEvent) {
                        if (keyEvent->code == Keyboard::Key::W && selectedIndex > 0) {
                            menuItems[selectedIndex].setFillColor(Color::White);
                            selectedIndex--;
                            menuItems[selectedIndex].setFillColor(Color::Red);
                        }
                        else if (keyEvent->code == Keyboard::Key::S && selectedIndex < 3) {
                            menuItems[selectedIndex].setFillColor(Color::White);
                            selectedIndex++;
                            menuItems[selectedIndex].setFillColor(Color::Red);
                        }
                        else if (keyEvent->code == Keyboard::Key::Enter) {
                            if (selectedIndex == 0) {
                                state = GameState::SaveSelect;
                            }
                            else if (selectedIndex == 1) {
                                state = GameState::Settings;
                            }
                            else if (selectedIndex == 2) {
                                state = GameState::Credits;
                            }
                            else {
                                window.close();
                            }
                        }
                    }
                }
            }
            else if (state == GameState::Settings) {
                if (event->is<Event::KeyPressed>()) {
                    auto keyEvent = event->getIf<Event::KeyPressed>();
                    if (keyEvent) {
                        if (keyEvent->code == Keyboard::Key::W && settingsIndex > 0)
                            settingsIndex--;
                        else if (keyEvent->code == Keyboard::Key::S && settingsIndex < 1)
                            settingsIndex++;
                        else if (keyEvent->code == Keyboard::Key::A && settingsIndex == 0) {
                            musicVolume = max(0.f, musicVolume - 5.f);
                            music.setVolume(musicVolume);
                        }
                        else if (keyEvent->code == Keyboard::Key::D && settingsIndex == 0) {
                            musicVolume = min(100.f, musicVolume + 5.f);
                            music.setVolume(musicVolume);
                        }
                        else if (keyEvent->code == Keyboard::Key::Enter && settingsIndex == 1) {
                            isFullscreen = !isFullscreen;
                            window.create(
                                isFullscreen ? VideoMode::getDesktopMode() : VideoMode(WINDOW_SIZE),
                                "Judgement Day",
                                isFullscreen ? State::Fullscreen : State::Windowed
                            );
                            window.setFramerateLimit(60);
                        }
                        else if (keyEvent->code == Keyboard::Key::Escape) {
                            state = GameState::Menu;
                        }
                    }
                }
            }
            else if (state == GameState::SaveSelect) {
                if (event->is<Event::KeyPressed>()) {
                    auto keyEvent = event->getIf<Event::KeyPressed>();
                    if (keyEvent) {
                        if (keyEvent->code == Keyboard::Key::W && saveSelectedIndex > 0) {
                            saveSlots[saveSelectedIndex].setFillColor(Color::White);
                            saveSelectedIndex--;
                            saveSlots[saveSelectedIndex].setFillColor(Color::Red);
                        }
                        else if (keyEvent->code == Keyboard::Key::S && saveSelectedIndex < 2) {
                            saveSlots[saveSelectedIndex].setFillColor(Color::White);
                            saveSelectedIndex++;
                            saveSlots[saveSelectedIndex].setFillColor(Color::Red);
                        }
                        else if (keyEvent->code == Keyboard::Key::Enter) {
                            SaveFile(saveSelectedIndex);
                            state = GameState::Game;
                        }
                        else if (keyEvent->code == Keyboard::Key::Escape) {
                            state = GameState::Menu;
                        }
                    }
                }
            }
        }

        window.clear();

        if (state == GameState::Menu) {
            window.draw(title);
            for (const auto& item : menuItems)
                window.draw(item);
        }
        else if (state == GameState::Game) {
            // Vector2f movement(0.f, 0.f);
            // handleInput(movement);
            // player.move(movement * dt);
            // constrainToWindow(player);

            window.draw(sprite);
            window.draw(player);
        }
        else if (state == GameState::Settings) {
            Vector2f settingsSize(WINDOW_SIZE.x / 2.f, WINDOW_SIZE.y / 2.f);
            Vector2f settingsPos(WINDOW_SIZE.x / 4.f, WINDOW_SIZE.y / 4.f);
            RectangleShape settingsBox(settingsSize);
            settingsBox.setPosition(settingsPos);
            settingsBox.setFillColor(Color::Black);
            settingsBox.setOutlineColor(Color::Red);
            settingsBox.setOutlineThickness(5.f);
        
            window.draw(settingsBox);
        
            Text setting1(font, "Volume", 60);
            setting1.setPosition(Vector2f(settingsPos.x + 50.f, settingsPos.y + 50.f));
            setting1.setFillColor(settingsIndex == 0 ? Color::Red : Color::White);
        
            Text setting2(font, "Window Mode", 60);
            setting2.setPosition(Vector2f(settingsPos.x + 50.f, settingsPos.y + 150.f));
            setting2.setFillColor(settingsIndex == 1 ? Color::Red : Color::White);
        
            window.draw(setting1);
            window.draw(setting2);
        
            RectangleShape barBack(Vector2f(300.f, 30.f));
            barBack.setPosition(Vector2f(settingsPos.x + 350.f, settingsPos.y + 75.f));
            barBack.setFillColor(Color::White);
        
            RectangleShape barFill(Vector2f(3.f * musicVolume, 30.f));
            barFill.setPosition(Vector2f(settingsPos.x + 350.f, settingsPos.y + 75.f));
            barFill.setFillColor(Color::Red);
        
            window.draw(barBack);
            window.draw(barFill);
        
            Text modeText(font, isFullscreen ? "Fullscreen" : "Windowed", 60);
            modeText.setPosition(Vector2f(settingsPos.x + 500.f, settingsPos.y + 150.f));
            window.draw(modeText);
        }
        else if (state == GameState::SaveSelect) {
            Text saveTitle(font, "Select Save", 108);
            saveTitle.setPosition(Vector2f(100.f, 200.f));
            saveTitle.setFillColor(Color::Red);
            window.draw(saveTitle);

            for (const auto& slot : saveSlots)
                window.draw(slot);
        }
        else {
            // Credits na końcu
        }

        window.display();
    }

    return 0;
}

