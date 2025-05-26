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
#include "map.hpp"
#include "worldmap.hpp"

using namespace sf;
using namespace std;

enum class GameState { Menu, Settings, Credits, Game, BossSelect, GameOptions, Guitar, Eufemia, World, EndScreen };

const float PLAYER_SIZE = 50.f;
const float MOVE_SPEED = 200.f;
const Vector2u WINDOW_SIZE(1920, 1080);

const int NUM_LANES = 6;
float NOTE_SPEED = 550.f;
const float SPAWN_INTERVAL = 0.3f;
const float NOTE_RADIUS = 20.f;
const float TARGET_Y = 900.f;
const float HIT_WINDOW = 50.f;
const int MAX_NOTES = 485;
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
    ContextSettings settings;
    Vector2u WINDOW_SIZE(1920, 1080);
    RenderWindow window(VideoMode(WINDOW_SIZE), "Judgement Day", State::Fullscreen, settings);
    window.setFramerateLimit(60);
    GameState state = GameState::Menu;

    bool isWin = false;
    bool playedSong = false;

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
    bool isGuitar = false;
    bool isEufemia = false;
    bool isWorld = false;
    float hitFlashTimers[NUM_LANES] = {0};
    const float HIT_FLASH_DURATION = 0.1f;

    float scoreFlashTimer = 0.f;
    bool scoreFlashing = false;

    Color normalMarkerColor = Color::White;
    Color flashColor = Color::Red;

    RectangleShape flashOverlay(Vector2f(1920, 1080));
    flashOverlay.setFillColor(Color(20,0,0));

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

    Text menuItems[3] = {
        Text(font, "Start", 108),
        Text(font, "Settings", 108),
        Text(font, "Exit", 108)
    };

    int selectedIndex = 0;
    Text title(font, "Judgement Day", 108);
    title.setPosition(Vector2f(WINDOW_SIZE.x - 1300.f, 20.f));
    title.setFillColor(Color::Red);

    for (int i = 0; i < 3; ++i) {
        menuItems[i].setPosition(Vector2f(40.f, 500.f + i * 110));
        menuItems[i].setFillColor(i == selectedIndex ? Color::Red : Color::White);
    }

    // RectangleShape player(Vector2f(PLAYER_SIZE, PLAYER_SIZE));
    // player.setFillColor(Color::Blue);
    // player.setPosition(Vector2f(375.f, 275.f));

    float musicVolume = 50.f;
    bool isFullscreen = true;
    
    Music music;
    if (music.openFromFile("assets\\sounds\\music\\RightBehindYou.wav")) {
        music.setLooping(true);
        music.setVolume(musicVolume);
        music.play();
    } else {
        cout << "Music not available!" << endl;
    }

    Music musicWorld;
    if (musicWorld.openFromFile("assets\\sounds\\music\\ThreeItems.wav")) {
        musicWorld.setLooping(true);
        musicWorld.setVolume(musicVolume);
    } else {
        cout << "Music not available!" << endl;
    }

    Music grandma20begin;
    if (grandma20begin.openFromFile("assets\\sounds\\music\\GrandmaTwoPointO_intro.wav")) {
        grandma20begin.setVolume(musicVolume);
    } else {
        cout << "Music not available!" << endl;
    }
    Music grandma20;
    if (grandma20.openFromFile("assets\\sounds\\music\\GrandmaTwoPointO_loop.wav")) {
        grandma20.setLooping(true);
        grandma20.setVolume(musicVolume);
    } else {
        cout << "Music not available!" << endl;
    }

    Music win;
    if (win.openFromFile("assets\\sounds\\music\\Win.wav")) {
        win.setVolume(musicVolume);
    } else {
        cout << "Music not available!" << endl;
    }

    Music begin;
    if (begin.openFromFile("assets\\sounds\\music\\Begin.wav")) {
        begin.setVolume(musicVolume);
    } else {
        cout << "Music not available!" << endl;
    }

    Music loss;
    if (loss.openFromFile("assets\\sounds\\music\\Loss.wav")) {
        loss.setVolume(musicVolume);
    } else {
        cout << "Music not available!" << endl;
    }

    Music rain;
    if (rain.openFromFile("assets\\sounds\\ambient\\rain.wav")) {
        rain.setLooping(true);
        rain.setVolume(75.f);
    } else {
        cout << "Music not available!" << endl;
    }

    Music guitar_music;
    if (guitar_music.openFromFile("assets\\sounds\\music\\MEDIC.wav")) {
        guitar_music.setVolume(musicVolume);
    } else {
        cout << "Medic not available!" << endl;
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

    Text bossSlots[3] = {
        Text(font, "Boss 1", 100),
        Text(font, "Boss 2", 100),
        Text(font, "Map", 100)
    };

    for (int i = 0; i < 3; ++i) {
        bossSlots[i].setPosition(Vector2f(100.f, 400.f + i * 150.f));
        bossSlots[i].setFillColor(i == BossSelectedIndex ? Color::Red : Color::White);
    }

    //Tu zaczyna się gra

    
    Hero player(100.f, WINDOW_SIZE.y - 150.f, 20);
    Boss witch(WINDOW_SIZE.x / 2.f - 75.f, 340.f, "assets/textures/entity/boss/eufemia/eufemia.png", 20);
    Crystal leftCrystal("assets/textures/entity/boss/eufemia/crystal.png", 220.f, 200.f, 120.f);
    Crystal rightCrystal("assets/textures/entity/boss/eufemia/crystal.png", WINDOW_SIZE.x - 220.f, 200.f, 120.f);

    vector<Projectile> enemyProjectiles;
    float shootTimer = 0.f;
    const float shootInterval = 1.f;

    Platform platform(Vector2f(WINDOW_SIZE.x, 50.f), Vector2f(0.f, WINDOW_SIZE.y - 50.f), Color(16, 16, 16));

    RectangleShape leftCrystalHpBack(Vector2f(150.f, 15.f));
    leftCrystalHpBack.setFillColor(Color(100, 100, 100));
    leftCrystalHpBack.setPosition(Vector2f(50.f, WINDOW_SIZE.y - 40.f));

    RectangleShape leftCrystalHp(Vector2f(150.f, 15.f));
    leftCrystalHp.setFillColor(Color::Cyan);
    leftCrystalHp.setPosition(leftCrystalHpBack.getPosition());

    RectangleShape rightCrystalHpBack(Vector2f(150.f, 15.f));
    rightCrystalHpBack.setFillColor(Color(100, 100, 100));
    rightCrystalHpBack.setPosition(Vector2f(WINDOW_SIZE.x - 200.f, WINDOW_SIZE.y - 40.f));

    RectangleShape rightCrystalHp(Vector2f(150.f, 15.f));
    rightCrystalHp.setFillColor(Color::Cyan);
    rightCrystalHp.setPosition(rightCrystalHpBack.getPosition());

    RectangleShape bossHpBack(Vector2f(300.f, 20.f));
    bossHpBack.setFillColor(Color(100, 100, 100));
    bossHpBack.setPosition(Vector2f(WINDOW_SIZE.x / 2.f - 150.f, WINDOW_SIZE.y - 40.f));

    RectangleShape bossHp(Vector2f(300.f, 20.f));
    bossHp.setFillColor(Color::Magenta);
    bossHp.setPosition(bossHpBack.getPosition());

    RectangleShape playerHpBack(Vector2f(200.f, 20.f));
    playerHpBack.setFillColor(Color(100, 100, 100));
    playerHpBack.setPosition(Vector2f(20.f, 20.f));

    RectangleShape playerHpBar(Vector2f(200.f, 20.f));
    playerHpBar.setFillColor(Color::Blue);
    playerHpBar.setPosition(playerHpBack.getPosition());

    bool playerHit = false;
    float hitTimer = 0.f;
    bool isBossPhaseTwo = false;

    Clock phaseTwoClock;
    bool phaseTwoAttackActive = true;
    Texture arenaT;
    if (!arenaT.loadFromFile("assets\\textures\\tile\\eufemia_arena.png", false, IntRect({0, 0}, {910, 512}))){
       cout << "Menu panorama not found!" << endl;
    }

    Sprite arena(arenaT);
    arena.setPosition({0.f, 0.f});
    arena.setScale({2.11f, 2.11f});

    bool isGame = false;
    string mainObjective = "";
    Clock clock;

    loadMap("source\\map\\level_data\\test_level.bmp");
    float dt = 0.1;

    while (window.isOpen()) {
        dt = clock.restart().asSeconds();
        while(auto event = window.pollEvent()){
            if(event->is<Event::Closed>())
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
                        else if (keyEvent->code == Keyboard::Key::S && selectedIndex < 2) {
                            menuItems[selectedIndex].setFillColor(Color::White);
                            selectedIndex++;
                            menuItems[selectedIndex].setFillColor(Color::Red);
                        }
                        else if (keyEvent->code == Keyboard::Key::Enter) {
                            if (selectedIndex == 0) {
                                state = GameState::World;
                                music.stop();
                                begin.play();
                            }
                            else if (selectedIndex == 1) {
                                state = GameState::Settings;
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
                        else if (keyEvent->code == Keyboard::Key::S && BossSelectedIndex < 2) {
                            bossSlots[BossSelectedIndex].setFillColor(Color::White);
                            BossSelectedIndex++;
                            bossSlots[BossSelectedIndex].setFillColor(Color::Red);
                        }
                        else if (keyEvent->code == Keyboard::Key::Enter) {
                            if(BossSelectedIndex == 0){
                                music.stop();
                                guitar_music.play();
                                gameOver = false;
                                misses = 0;
                                state = GameState::Guitar;
                            }else if(BossSelectedIndex == 1){
                                music.stop();
                                state = GameState::Eufemia;
                            }else if(BossSelectedIndex == 2){
                                music.stop();
                                state = GameState::World;
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
                                if (isGuitar == true){
                                    state = GameState::Guitar;
                                }
                                if (isEufemia == true){
                                    state = GameState::Eufemia;
                                }
                                if (isWorld == true){
                                    state = GameState::World;
                                }
                            }
                        else if (gameOptionsSelectedIdex == 1){
                                
                                state = GameState::Settings;
                            }
                            else if (gameOptionsSelectedIdex == 2){
                                music.play();
                                state = GameState::Menu;
                            }
                        }
                        else if (keyEvent->code == Keyboard::Key::Escape) {
                            state = GameState::Game;
                        }
                    }
                }
            }else if(state == GameState::EndScreen){
                music.stop();
                musicWorld.stop();
                guitar_music.stop();
                begin.stop();
                grandma20.stop();
                grandma20begin.stop();
                if(event->is<Event::MouseButtonPressed>()){
                    if(isWin){
                        state = GameState::World;
                    }else{
                        window.close();
                    }
                }
            }
        }
        
        if (state == GameState::Guitar){
                            isGuitar = true;
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
                isWin = false;
                resultText.setString("Game Over");
                FloatRect textBounds = resultText.getLocalBounds();
                resultText.setOrigin(Vector2f(textBounds.position.x / 2.f, textBounds.position.y / 2.f));
                resultText.setPosition(Vector2f(1920 / 2.f, 1080 / 2.f));
                guitar_music.stop();
            } else if (notesSpawned == MAX_NOTES && notes.empty()) {
                isWin = true;
                gameOver = true;
                victory = true;
                resultText.setString("Boss Defeated!");
                FloatRect textBounds = resultText.getLocalBounds();
                resultText.setOrigin(Vector2f(textBounds.position.x / 2.f, textBounds.position.y / 2.f));
                resultText.setPosition(Vector2f(1920 / 2.f, 1080 / 2.f));
                guitar_music.stop();
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
            } else {
                screenFlash = false;
                flashTimer = 0.f;
            }

                }else if(state == GameState::Eufemia){

                    if(grandma20begin.getStatus() == SoundSource::Status::Stopped && grandma20.getStatus() == SoundSource::Status::Stopped)
                        grandma20.play();

                    isEufemia = true;
                    float deltaTime = dt;
                    playerHpBar.setSize(Vector2f(200.f * (float)player.hp / 20.f, 20.f));

                    if(player.hp <= 0){
                        state = GameState::EndScreen;
                    }else if (witch.getHP() <= 0)
                    {
                        isWin = true;
                        state = GameState::EndScreen;
                    }
                    
                    player.handleMovement(deltaTime, 300.f, window.getSize());
                    player.handleJumping(deltaTime, WINDOW_SIZE.y - 150.f);

                    if (Mouse::isButtonPressed(Mouse::Button::Left)) {
                        player.shoot();
                    }

                    shootTimer += deltaTime;
                    if (shootTimer >= shootInterval) {
                        shootTimer = 0.f;

                        if (leftCrystal.isAlive()) {
                            Vector2f dir = player.shape.getPosition() - leftCrystal.sprite->getPosition();
                            float len = sqrt(dir.x * dir.x + dir.y * dir.y);
                            if (len != 0) dir /= len;
                            enemyProjectiles.emplace_back(leftCrystal.sprite->getPosition().x, leftCrystal.sprite->getPosition().y, dir * 400.f);
                        }
                        if (rightCrystal.isAlive()) {
                            Vector2f dir = player.shape.getPosition() - rightCrystal.sprite->getPosition();
                            float len = sqrt(dir.x * dir.x + dir.y * dir.y);
                            if (len != 0) dir /= len;
                            enemyProjectiles.emplace_back(rightCrystal.sprite->getPosition().x, rightCrystal.sprite->getPosition().y, dir * 400.f);
                        }
                    }

                    player.updateProjectiles2(deltaTime);

                    for (auto& proj : enemyProjectiles) {
                        proj.update(deltaTime);
                    }

                    for (auto i = enemyProjectiles.begin(); i != enemyProjectiles.end();) {
                        if (i->shape.getGlobalBounds().findIntersection(player.shape.getGlobalBounds())) {
                            player.hp -= 1;
                            playerHit = true;
                            hitTimer = 0.2f;
                            i = enemyProjectiles.erase(i);
                        } else {
                            ++i;
                        }
                    }

                    for (auto i = player.projectiles.begin(); i != player.projectiles.end();) {
                        bool erased = false;

                        if (leftCrystal.isAlive() && i->shape.getGlobalBounds().findIntersection(leftCrystal.sprite->getGlobalBounds())) {
                            leftCrystal.takeDamage(1);
                            i = player.projectiles.erase(i);
                            erased = true;
                        }
                        else if (rightCrystal.isAlive() && i->shape.getGlobalBounds().findIntersection(rightCrystal.sprite->getGlobalBounds())) {
                            rightCrystal.takeDamage(1);
                            i = player.projectiles.erase(i);
                            erased = true;
                        }
                        else if (isBossPhaseTwo && witch.isAlive() && i->shape.getGlobalBounds().findIntersection(witch.sprite->getGlobalBounds())) {
                            witch.takeDamage(1);
                            i = player.projectiles.erase(i);
                            erased = true;
                        }

                        if (!erased) {
                            ++i;
                        }
                    }

                    if (!leftCrystal.isAlive() && !rightCrystal.isAlive()) {
                        if (!isBossPhaseTwo) {
                            isBossPhaseTwo = true;
                            phaseTwoClock.restart();
                            phaseTwoAttackActive = true;
                        }
                    }

                    if (isBossPhaseTwo && witch.isAlive()) {
                        float phaseTime = phaseTwoClock.getElapsedTime().asSeconds();
                        if (phaseTime >= 3.f && phaseTime < 6.f) {
                            phaseTwoAttackActive = false;
                        } else if (phaseTime >= 6.f) {
                            phaseTwoClock.restart();
                            phaseTwoAttackActive = true;
                        }

                        if (phaseTwoAttackActive) {
                            witch.phaseTwoAttack(player, enemyProjectiles);
                        }
                    }

                    if (hitTimer > 0.f) {
                        hitTimer -= deltaTime;
                        player.shape.setFillColor(Color::Red);
                    } else {
                        player.shape.setFillColor(Color::Blue);
                    }

                    leftCrystal.update(deltaTime);
                    rightCrystal.update(deltaTime);

                    leftCrystalHp.setSize(Vector2f(150.f * (float)leftCrystal.getHP() / 5.f, 15.f));
                    rightCrystalHp.setSize(Vector2f(150.f * (float)rightCrystal.getHP() / 5.f, 15.f));
                    bossHp.setSize(Vector2f(300.f * (float)witch.getHP() / 20.f, 20.f));

                }else if(state == GameState::World){
                    isWorld = true;

                    if(Keyboard::isKeyPressed(Keyboard::Scan::Escape))
                        state = GameState::GameOptions;
                    
                    if(begin.getStatus() == SoundSource::Status::Stopped && musicWorld.getStatus() == SoundSource::Status::Stopped){
                        musicWorld.play();
                    }

                    worldTick(dt);

                    if(checkEufemia()){
                        state = GameState::Eufemia;
                        isWorld = false;
                        isEufemia = true;
                        begin.stop();
                        musicWorld.stop();
                        grandma20begin.play();

                        loadMap("source\\map\\level_data\\test_level.bmp");
                    }
                    
                    if(checkBlack()){
                        state = GameState::Guitar;
                        isWorld = false;
                        isGuitar = true;
                        guitar_music.play();
                        begin.stop();
                        musicWorld.stop();
                        loadMap("source\\map\\level_data\\test_level.bmp");
                    }
                }
            
        
        window.clear(Color::Black);
                
        if (state == GameState::Menu) {
            isGuitar = false;
            isEufemia = false;
            isWorld = false;
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
            if (gameOver){
                state = GameState::EndScreen;
            }
            if (screenFlash)
                window.draw(flashOverlay);
        }
        else if (state == GameState::Eufemia) {
window.draw(arena);
        window.draw(platform.shape);
        window.draw(playerHpBack);
        window.draw(playerHpBar);
        if (witch.sprite) window.draw(*witch.sprite);

        if (leftCrystal.isAlive()) window.draw(*leftCrystal.sprite);
        if (rightCrystal.isAlive()) window.draw(*rightCrystal.sprite);

        for (auto& proj : player.projectiles) {
            proj.shape.setFillColor(Color::Yellow);
            window.draw(proj.shape);
        }
        for (auto& proj : enemyProjectiles) {
            proj.shape.setFillColor(Color::Red);
            window.draw(proj.shape);
        }

        player.draw(window);

        window.draw(leftCrystalHpBack);
        window.draw(leftCrystalHp);

        window.draw(rightCrystalHpBack);
        window.draw(rightCrystalHp);

        window.draw(bossHpBack);
        window.draw(bossHp);
        }else if(state == GameState::World){
            displayWorld(window, WINDOW_SIZE, dt);
        }else if (state == GameState::Settings) {
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
        }else if(state == GameState::EndScreen){
            Text m_message(font);
            m_message.setCharacterSize(48);
            m_message.setStyle(Text::Bold);
            if(isWin){
                if(win.getStatus() == SoundSource::Status::Stopped && !playedSong){
                    win.play();
                    playedSong = true;
                }
                m_message.setString("Congratulations, victory! \n Press Mouse to continue.");
                m_message.setFillColor(Color::Green);
            }else{
                if(loss.getStatus() == SoundSource::Status::Stopped && !playedSong){
                    loss.play();
                    playedSong = true;
                }
                m_message.setString("Congratulations, you are a failure! \n Press Mouse to continue.");
                m_message.setFillColor(Color::Red);
            }

        FloatRect textRect = m_message.getLocalBounds();
        m_message.setOrigin(Vector2f(textRect.position.x + textRect.size.x / 2.0f,
                            textRect.position.y + textRect.size.y / 2.0f));
        m_message.setPosition(Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));
            window.draw(m_message);
        }
        window.display();

        // if(Keyboard::isKeyPressed(Keyboard::Scan::Escape))
            // window.close();
    }

    return 0;
}

/* Przykład użycia klasy player w main.cpp
#include <SFML/Graphics.hpp>
#include "Player.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML 3 RPG Player");
    Player player;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        player.handleInput();

        window.clear();
        player.draw(window);
        window.display();
    }

    return 0;
}
*/
/* Przykład użycia klasy npc w main.cpp
#include <SFML/Graphics.hpp>
#include "npc.hpp"
#include <variant>

int main() {
    sf::RenderWindow window(sf::VideoMode({800u, 600u}), "NPC Trigger - SFML 3.0.0");
    window.setFramerateLimit(60);

    sf::RectangleShape player(sf::Vector2f(40, 40));
    player.setFillColor(sf::Color::Blue);
    player.setPosition({100.f, 100.f});

    NPC npc({300.f, 300.f}, {50.f, 50.f});

    sf::Clock clock;

    while (window.isOpen()) {
        // Obsługa zdarzeń
        while (auto eventOpt = window.pollEvent()) {
            const sf::Event& event = *eventOpt;

            // Sprawdzenie czy event to zamknięcie okna
            if (std::get_if<sf::Event::Closed>(&event)) {
                window.close();
            }
        }

        // Poruszanie graczem
        float dt = clock.restart().asSeconds();
        sf::Vector2f movement(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W)) movement.y -= 200 * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) movement.y += 200 * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) movement.x -= 200 * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) movement.x += 200 * dt;
        player.move(movement);

        // Sprawdzenie kolizji z NPC
        npc.checkCollision(player);

        // Renderowanie
        window.clear();
        window.draw(player);
        npc.draw(window);
        window.display();
    }

    return 0;
}
*/
