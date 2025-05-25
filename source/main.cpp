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

enum class GameState { Menu, Settings, Credits, Game, BossSelect, GameOptions, Guitar, Eufemia, MapTest, EndScreen };

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
    bool isGuitar = false;
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
    bool isFullscreen = false;
    
    Music music;
    if (music.openFromFile("assets\\sounds\\music\\RightBehindYou.wav")) {
        music.setLooping(true);
        music.setVolume(musicVolume);
        // music.play();
    } else {
        cout << "Music not available!" << endl;
    }

    Music musicWorld;
    if (musicWorld.openFromFile("assets\\sounds\\music\\ThreeItems.wav")) {
        musicWorld.setLooping(true);
        musicWorld.setVolume(musicVolume);
        musicWorld.play();
    } else {
        cout << "Music not available!" << endl;
    }

    Music rain;
    if (rain.openFromFile("assets\\sounds\\ambient\\rain.wav")) {
        rain.setLooping(true);
        rain.setVolume(75.f);
        rain.play();
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

    loadMap("source\\map\\level_data\\test_level.bmp");

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while(const std::optional event = window.pollEvent()){
            if(event->is<Event::Closed>())
                window.close();
        }

        if(Keyboard::isKeyPressed(Keyboard::Scan::Escape))
            window.close();
        
        worldTick(dt);

        window.clear();

        displayWorld(window, WINDOW_SIZE, dt);

        window.display();
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
