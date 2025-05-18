#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <cmath>
#include <algorithm>
#include "hero.hpp"
#include "bosses.hpp"
#include "projectile.hpp"
#include "crystal.hpp"
#include "ui.hpp"
#include "platform.hpp"

using namespace sf;
using namespace std;

enum class GameState { Menu, Settings, Credits, Game, BossSelect, GameOptions, Guitar, Eufemia };

const float PLAYER_SIZE = 50.f;
const float MOVE_SPEED = 200.f;
const Vector2u WINDOW_SIZE(1920, 1080);

const int NUM_LANES = 6;
float NOTE_SPEED = 550.f;
const float SPAWN_INTERVAL = 0.3f;
const float NOTE_RADIUS = 20.f;
const float TARGET_Y = 900.f;
const float HIT_WINDOW = 50.f;
const int MAX_NOTES = 1000;
const int MAX_MISSES = 20;



struct Note {
    CircleShape shape;
    int lane;
    bool active = true;

    Note(int lane, float startY) : lane(lane) {
        shape.setRadius(NOTE_RADIUS);
        shape.setOrigin(Vector2f(NOTE_RADIUS, NOTE_RADIUS));
        shape.setFillColor(Color::Red);
        float laneWidth = 160.f;
        shape.setPosition(Vector2f(100.f + lane * laneWidth + 80.f, startY));
    }

    void update(float dt) {
        shape.move(Vector2f(0, NOTE_SPEED * dt));
        if (shape.getPosition().y > 1080)
            active = false;
    }
};

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

int main() {
    RenderWindow window(VideoMode(WINDOW_SIZE), "Judgement Day");
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(0)));
    GameState state = GameState::Menu;
    //Menu i grafika

    Texture horse;
    if (!horse.loadFromFile("assets\\textures\\horse.png")){
       cout << "Horse not found!" << endl;
       return -1;
    }

    
    
    vector<Note> notes;
    vector<Text> keyTexts;
    vector<CircleShape> hitMarkers;

    notes.reserve(MAX_NOTES);
    map<int, Keyboard::Key> laneKeys = {
        {0, Keyboard::Key::A},
        {1, Keyboard::Key::S},
        {2, Keyboard::Key::D},
        {3, Keyboard::Key::J},
        {4, Keyboard::Key::K},
        {5, Keyboard::Key::L}
    };

    map<int, string> keyLabels = {
        {0, "A"},
        {1, "S"},
        {2, "D"},
        {3, "J"},
        {4, "K"},
        {5, "L"}
    };



    Font font;
    if (!font.openFromFile("assets\\fonts\\font1.otf")) {
        cout << "Font not available!" << endl;
    }

    Text scoreText(font, "", 32);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(Vector2f(20.f, 20.f));

    Text resultText(font, "", 80);
    resultText.setFillColor(Color::Red);
    FloatRect resultBounds = resultText.getLocalBounds();
    resultText.setOrigin(Vector2f(resultBounds.position.x / 2, resultBounds.position.y / 2));
    resultText.setPosition(Vector2f(1920 / 2.f, 1080 / 2.f));

    RectangleShape targetBar(Vector2f(160.f * NUM_LANES, 10.f));
    targetBar.setFillColor(Color::White);
    targetBar.setPosition(Vector2f(100.f, TARGET_Y));

    for (int i = 0; i < NUM_LANES; ++i) {
        Text keyText(font, keyLabels[i], 36);
        keyText.setFillColor(Color::White);
        keyText.setPosition(Vector2f(100.f + i * 160.f + 70.f, TARGET_Y + 40.f));
        keyTexts.push_back(keyText);

        CircleShape hitMarker(NOTE_RADIUS);
        hitMarker.setOrigin(Vector2f(NOTE_RADIUS, NOTE_RADIUS));
        hitMarker.setFillColor(Color::White);
        hitMarker.setPosition(Vector2f(100.f + i * 160.f + 80.f, TARGET_Y));
        hitMarkers.push_back(hitMarker);
    }

    float spawnTimer = 0.f;
    int notesSpawned = 0;
    int score = 0;
    int misses = 0;
    bool gameOver = false;
    bool victory = false;
    int streak = 0;
    bool screenFlash = false;
    float flashTimer = 0.f;

    float hitFlashTimers[NUM_LANES] = {0};
    const float HIT_FLASH_DURATION = 0.1f;

    float scoreFlashTimer = 0.f;
    bool scoreFlashing = false;

    Color normalMarkerColor = Color::White;
    Color flashColor = Color::Red;

    RectangleShape flashOverlay(Vector2f(1920, 1080));
    flashOverlay.setFillColor(Color::Red);

    Text gameOptions[4] = {
        Text(font, "Resume game", 80),
        Text(font, "Options", 80),
        Text(font, "Save/Main menu", 80),
        Text(font, "Main objective", 80)
    };

    int gameOptionsSelectedIdex = 0;
    for (int i = 0; i < 3; ++i) {
        gameOptions[i].setPosition(Vector2f(100.f, 400.f + i * 150.f));
        gameOptions[i].setFillColor(i == gameOptionsSelectedIdex ? Color::Red : Color::White);
    }

    Text settingsItems[2] = {
        Text(font, "Volume", 60),
        Text(font, "Toggle Fullscreen", 60)
    };

    int selectedSettingsIndex = 0;

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

    // RectangleShape player(Vector2f(PLAYER_SIZE, PLAYER_SIZE));
    // player.setFillColor(Color::Blue);
    // player.setPosition(Vector2f(375.f, 275.f));

    float musicVolume = 50.f;
    bool isFullscreen = false;
    
    Music music;
    if (music.openFromFile("assets\\sounds\\music\\RightBehindYou.wav")) {
        music.setLooping(true);
        music.setVolume(musicVolume);
        music.play();
    } else {
        cout << "Music not available!" << endl;
    }

    Texture panorama_texture;
    if (!panorama_texture.loadFromFile("assets\\textures\\gui\\menu_panorama.png", false, IntRect({0, 0}, {544, 288}))){
       cout << "Menu panorama not found!" << endl;
    }

    Sprite panorama(panorama_texture);
    panorama.setPosition({-16.f, -16.f});
    panorama.setScale({3.75f, 3.75f});

    VertexArray options_fade(PrimitiveType::TriangleStrip, 4);

    options_fade[0].position = Vector2f(0.f, 475.f);
    options_fade[1].position = Vector2f(0.f, 1080.f);
    options_fade[2].position = Vector2f(475.f, 475.f);
    options_fade[3].position = Vector2f(475.f, 1080.f);

    options_fade[0].color = Color::Black;
    options_fade[1].color = Color::Black;
    options_fade[2].color = Color(0, 0, 0, 0);
    options_fade[3].color = Color(0, 0, 0, 0);

    int BossSelectedIndex = 0;

    Text bossSlots[2] = {
        Text(font, "Boss 1", 100),
        Text(font, "Boss 2", 100)
    };

    for (int i = 0; i < 2; ++i) {
        bossSlots[i].setPosition(Vector2f(100.f, 400.f + i * 150.f));
        bossSlots[i].setFillColor(i == BossSelectedIndex ? Color::Red : Color::White);
    }

    //Tu zaczyna się gra


    Hero player(100.f, WINDOW_SIZE.y - 150.f, 20);
    Boss witch(WINDOW_SIZE.x / 2.f - 30.f, 440.f, Color::Green, 20);
    witch.shape.setSize({60.f, 100.f});

    Crystal leftCrystal(100.f, 150.f);
    Crystal rightCrystal(WINDOW_SIZE.x - 130.f, 150.f);

    vector<Projectile> projectiles;
    float shootTimer = 0.f;
    const float shootInterval = 1.f;

    Platform platform(Vector2f(WINDOW_SIZE.x, 50.f), Vector2f(0.f, WINDOW_SIZE.y - 50.f), Color(50, 205, 50));
    UI hpBar(WINDOW_SIZE, player.hp, witch.hp);

    bool onGround = false;
    bool isShooting = false;
    bool playerHit = false;
    float hitTimer = 0.f;
    bool isBossPhaseTwo = false;

    bool isGame = false;
    string mainObjective = "";
    Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        window.clear(Color::Black);
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
                                state = GameState::BossSelect;
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
                            if (keyEvent->code == Keyboard::Key::W && selectedSettingsIndex > 0) {
                                settingsItems[selectedSettingsIndex].setFillColor(Color::White);
                                selectedSettingsIndex--;
                                settingsItems[selectedSettingsIndex].setFillColor(Color::Red);
                            }
                            else if (keyEvent->code == Keyboard::Key::S && selectedSettingsIndex < 1) {
                                settingsItems[selectedSettingsIndex].setFillColor(Color::White);
                                selectedSettingsIndex++;
                                settingsItems[selectedSettingsIndex].setFillColor(Color::Red);
                            }
                            else if (keyEvent->code == Keyboard::Key::A && selectedSettingsIndex == 0) {
                                musicVolume = max(0.f, musicVolume - 5.f);
                                music.setVolume(musicVolume);
                            }
                            else if (keyEvent->code == Keyboard::Key::D && selectedSettingsIndex == 0) {
                                musicVolume = min(100.f, musicVolume + 5.f);
                                music.setVolume(musicVolume);
                            }
                            else if (keyEvent->code == Keyboard::Key::Enter && selectedSettingsIndex == 1) {
                            isFullscreen = !isFullscreen;
                            window.create(
                                isFullscreen ? VideoMode::getDesktopMode() : VideoMode(WINDOW_SIZE),
                                "Judgement Day",
                                isFullscreen ? State::Fullscreen : State::Windowed
                            );
                            window.setFramerateLimit(60);
                        }
                        else if (keyEvent->code == Keyboard::Key::Escape) {
                            state = isGame ? GameState::GameOptions : GameState::Menu;
                        }
                    }
                }
            }
            else if (state == GameState::BossSelect) {
                if (event->is<Event::KeyPressed>()) {
                    auto keyEvent = event->getIf<Event::KeyPressed>();
                    if (keyEvent) {
                        if (keyEvent->code == Keyboard::Key::W && BossSelectedIndex > 0) {
                            bossSlots[BossSelectedIndex].setFillColor(Color::White);
                            BossSelectedIndex--;
                            bossSlots[BossSelectedIndex].setFillColor(Color::Red);
                        }
                        else if (keyEvent->code == Keyboard::Key::S && BossSelectedIndex < 1) {
                            bossSlots[BossSelectedIndex].setFillColor(Color::White);
                            BossSelectedIndex++;
                            bossSlots[BossSelectedIndex].setFillColor(Color::Red);
                        }
                        else if (keyEvent->code == Keyboard::Key::Enter) {
                            if(BossSelectedIndex == 0){
                                state = GameState::Guitar;
                            }else if(BossSelectedIndex == 1){
                                state = GameState::Eufemia;
                            }
                        }
                        else if (keyEvent->code == Keyboard::Key::Escape) {
                            state = GameState::Menu;
                        }
                    }
                }
            }
            else if (state == GameState::Game){
                auto keyEvent = event->getIf<Event::KeyPressed>();
                    if (keyEvent) {
                        if (keyEvent->code == Keyboard::Key::Escape){
                            state = GameState::GameOptions; 
                        }
                }
            }
            else if (state == GameState::GameOptions){
                if (event->is<Event::KeyPressed>()) {
                    auto keyEvent = event->getIf<Event::KeyPressed>();
                    if (keyEvent) {
                        if (keyEvent->code == Keyboard::Key::W && gameOptionsSelectedIdex > 0) {
                            gameOptions[gameOptionsSelectedIdex].setFillColor(Color::White);
                            gameOptionsSelectedIdex--;
                            gameOptions[gameOptionsSelectedIdex].setFillColor(Color::Red);
                        }
                        else if (keyEvent->code == Keyboard::Key::S && gameOptionsSelectedIdex < 2) {
                            gameOptions[gameOptionsSelectedIdex].setFillColor(Color::White);
                            gameOptionsSelectedIdex++;
                            gameOptions[gameOptionsSelectedIdex].setFillColor(Color::Red);
                        }
                        else if (keyEvent->code == Keyboard::Key::Enter) {
                            if (gameOptionsSelectedIdex == 0) {
                                state = GameState::Game;
                            }
                        else if (gameOptionsSelectedIdex == 1){
                                
                                state = GameState::Settings;
                            }
                            else if (gameOptionsSelectedIdex == 2){
                                state = GameState::Menu;
                            }
                        }
                        else if (keyEvent->code == Keyboard::Key::Escape) {
                            state = GameState::Game;
                        }
                    }
                }
            } else if (state == GameState::Guitar){
            if (!gameOver && notesSpawned < MAX_NOTES) {
                spawnTimer += dt;
                while (spawnTimer >= SPAWN_INTERVAL && notesSpawned < MAX_NOTES) {
                    spawnTimer -= SPAWN_INTERVAL;
                    int lane = rand() % NUM_LANES;
                    notes.emplace_back(lane, -NOTE_RADIUS);
                    notesSpawned++;
                }
            }

            for (auto& n : notes)
                n.update(dt);

            for (int lane = 0; lane < NUM_LANES; ++lane) {
                if (Keyboard::isKeyPressed(laneKeys[lane])) {
                    for (auto& note : notes) {
                        if (note.lane == lane && note.active) {
                            float yDist = abs(note.shape.getPosition().y - TARGET_Y);
                            if (yDist <= HIT_WINDOW) {
                                note.active = false;
                                score += 100;
                                streak++;
                                hitFlashTimers[lane] = HIT_FLASH_DURATION;
                                scoreFlashTimer = HIT_FLASH_DURATION;
                                scoreFlashing = true;
                                break;
                            }
                        }
                    }
                }
            }

            for (auto& note : notes) {
                if (note.active && note.shape.getPosition().y > TARGET_Y + HIT_WINDOW) {
                    note.active = false;
                    misses++;
                    streak = 0;
                    NOTE_SPEED = 550.f;
                }
            }

            notes.erase(remove_if(notes.begin(), notes.end(), [](Note& n) { return !n.active; }), notes.end());

            bool anyLaneFlashing = false;
            for (int i = 0; i < NUM_LANES; ++i) {
                if (hitFlashTimers[i] > 0) {
                    hitFlashTimers[i] -= dt;
                    hitMarkers[i].setFillColor(flashColor);
                    anyLaneFlashing = true;
                } else {
                    hitMarkers[i].setFillColor(normalMarkerColor);
                }
            }

            targetBar.setFillColor(anyLaneFlashing ? flashColor : normalMarkerColor);

            if (scoreFlashing) {
                if (scoreFlashTimer > 0) {
                    scoreFlashTimer -= dt;
                    scoreText.setFillColor(flashColor);
                } else {
                    scoreFlashing = false;
                    scoreText.setFillColor(normalMarkerColor);
                }
            }

            scoreText.setString("Score: " + to_string(score) + "    Misses: " + to_string(misses) + "/" + to_string(MAX_MISSES) + "    Streak: " + to_string(streak));

            if (misses >= MAX_MISSES) {
                gameOver = true;
                resultText.setString("Game Over");
                FloatRect textBounds = resultText.getLocalBounds();
                resultText.setOrigin(Vector2f(textBounds.position.x / 2.f, textBounds.position.y / 2.f));
                resultText.setPosition(Vector2f(1920 / 2.f, 1080 / 2.f));
            } else if (notesSpawned == MAX_NOTES && notes.empty()) {
                gameOver = true;
                victory = true;
                resultText.setString("Boss Defeated!");
                FloatRect textBounds = resultText.getLocalBounds();
                resultText.setOrigin(Vector2f(textBounds.position.x / 2.f, textBounds.position.y / 2.f));
                resultText.setPosition(Vector2f(1920 / 2.f, 1080 / 2.f));
            }

            if (streak >= 60) {
                flashTimer += dt;
                if (flashTimer >= 0.2f) {
                    flashTimer = 0.f;
                    screenFlash = !screenFlash;
                }
            } else if (streak == 20) {
                NOTE_SPEED = 600.f;
            } else if (streak == 40) {
                NOTE_SPEED = 700.f;
            }

                }else if(state == GameState::Eufemia){
                    float deltaTime = clock.restart().asSeconds();
                    
                    player.handleMovement(deltaTime, 200.f, window.getSize());
                    player.handleJumping(deltaTime, WINDOW_SIZE.y - 150.f);
                    
                    shootTimer += deltaTime;
                    if (shootTimer >= shootInterval) {
                        shootTimer = 0.f;
                        if (leftCrystal.alive) {
                            Vector2f dir = player.shape.getPosition() - leftCrystal.shape.getPosition();
                            projectiles.push_back(Projectile(leftCrystal.shape.getPosition().x, leftCrystal.shape.getPosition().y, dir * 400.f));
                        }
                        if (rightCrystal.alive) {
                            Vector2f dir = player.shape.getPosition() - rightCrystal.shape.getPosition();
                            projectiles.push_back(Projectile(rightCrystal.shape.getPosition().x, rightCrystal.shape.getPosition().y, dir * 400.f));
                        }
                    }

                    for (auto& proj : projectiles) {
                        proj.update(deltaTime);
                    }

                    for (auto i = projectiles.begin(); i != projectiles.end();) {
                        if (i->shape.getGlobalBounds().findIntersection(player.shape.getGlobalBounds())) {
                            player.hp -= 1;
                            playerHit = true;
                            hitTimer = 0.2f;
                            i = projectiles.erase(i);
                        } else {
                            ++i;
                        }
                    }

                    if (!leftCrystal.alive || !rightCrystal.alive) {
                        witch.shape.setFillColor(Color::Red);
                        if (!isBossPhaseTwo) {
                            isBossPhaseTwo = true;
                        }
                    }

                    if (isBossPhaseTwo) {
                        witch.phaseTwoAttack(player, projectiles);
                    }

                    if (hitTimer > 0.f) {
                        hitTimer -= deltaTime;
                        player.shape.setFillColor(Color::Red);
                    } else {
                        player.shape.setFillColor(Color::Blue);
                    }
                }
            }
    
        

        window.clear();

        if (state == GameState::Menu) {
            isGame = false;
            window.draw(panorama);
            window.draw(options_fade);
            window.draw(title);
            for (const auto& item : menuItems)
                window.draw(item);
        }
        else if (state == GameState::Guitar) {
            isGame = true;
            window.clear(Color::Black);
            window.draw(targetBar);
            for (auto& hitMarker : hitMarkers)
                window.draw(hitMarker);
            for (auto& keyText : keyTexts)
                window.draw(keyText);
            for (auto& n : notes)
                window.draw(n.shape);
            window.draw(scoreText);
            if (gameOver)
                window.draw(resultText);
            if (screenFlash)
                window.draw(flashOverlay);
        }
        else if (state == GameState::Eufemia) {


            hpBar.update(player.hp, witch.hp);

            window.draw(platform.shape);
            window.draw(witch.shape);
            window.draw(hpBar.hpBarBack);
            window.draw(hpBar.hpBar);
            if (leftCrystal.alive) window.draw(leftCrystal.shape);
            if (rightCrystal.alive) window.draw(rightCrystal.shape);
            
            for (auto& proj : projectiles) {
                proj.shape.setFillColor(Color(rand() % 255, rand() % 255, rand() % 255));
                window.draw(proj.shape);
            }
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
        
            for (int i = 0; i < 2; ++i) {
                settingsItems[i].setPosition(Vector2f(settingsPos.x + 50.f, settingsPos.y + 50.f + i * 100.f));
                settingsItems[i].setFillColor(i == selectedSettingsIndex ? Color::Red : Color::White);
                window.draw(settingsItems[i]);
            }

            RectangleShape barBack(Vector2f(300.f, 30.f));
            barBack.setPosition(Vector2f(settingsPos.x + 350.f, settingsPos.y + 75.f));
            barBack.setFillColor(Color::White);
        
            RectangleShape barFill(Vector2f(3.f * musicVolume, 30.f));
            barFill.setPosition(Vector2f(settingsPos.x + 350.f, settingsPos.y + 75.f));
            barFill.setFillColor(Color::Red);
        
            window.draw(barBack);
            window.draw(barFill);
        
            Text modeText(font, isFullscreen ? "Fullscreen" : "Windowed", 60);
            modeText.setPosition(Vector2f(settingsPos.x + 550.f, settingsPos.y + 150.f));
            window.draw(modeText);
        }
        else if (state == GameState::BossSelect) {
            Text saveTitle(font, "Select Boss", 108);
            saveTitle.setPosition(Vector2f(100.f, 200.f));
            saveTitle.setFillColor(Color::Red);
            window.draw(saveTitle);

            for (const auto& slot : bossSlots)
                window.draw(slot);
        }
        else if (state == GameState::GameOptions) {
            Vector2f settingsSize(WINDOW_SIZE.x / 2.f, WINDOW_SIZE.y / 2.f);
            Vector2f settingsPos(WINDOW_SIZE.x / 4.f, WINDOW_SIZE.y / 4.f);
            RectangleShape settingsBox(settingsSize);
            settingsBox.setPosition(settingsPos);
            settingsBox.setFillColor(Color::Black);
            settingsBox.setOutlineColor(Color::Red);
            settingsBox.setOutlineThickness(5.f);
            RectangleShape line(Vector2f(settingsSize.x - 60.f, 3.f));
            line.setPosition(Vector2f(settingsPos.x + 30.f, settingsPos.y + 55.f + 3 * 100.f));
            line.setFillColor(Color::Red);

            window.draw(settingsBox);

            for (int i = 0; i < 4; ++i) {
                gameOptions[i].setPosition(Vector2f(settingsPos.x + 50.f, settingsPos.y + 50.f + i * 100.f));
                if (i != 3){
                    gameOptions[i].setFillColor(i == gameOptionsSelectedIdex ? Color::Red : Color::White);
                }
                else {
                    gameOptions[i].setFillColor(Color::Red);
                }
                window.draw(gameOptions[i]);
            }
            window.draw(line);
        }
        else {
            // Credits na końcu
        }

        window.display();
    }

    return 0;
}
