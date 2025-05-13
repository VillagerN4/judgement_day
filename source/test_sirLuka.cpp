#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
using namespace sf;
using namespace std;

const int NUM_LANES = 6;
const float NOTE_SPEED = 300.f;
const float SPAWN_INTERVAL = 0.7f;
const Vector2f NOTE_SIZE(40.f, 40.f);
const float TARGET_Y = 900.f;
const float HIT_WINDOW = 40.f;

map<int, Keyboard::Key> laneKeys = {
    {0, Keyboard::Key::A},
    {1, Keyboard::Key::S},
    {2, Keyboard::Key::D},
    {3, Keyboard::Key::J},
    {4, Keyboard::Key::K},
    {5, Keyboard::Key::L}
};

struct Note {
    RectangleShape shape;
    int lane;
    bool active = true;

    Note(int lane, float startY) : lane(lane) {
        shape.setSize(NOTE_SIZE);
        shape.setFillColor(Color::Red);
        float laneWidth = 160.f;
        shape.setPosition(Vector2f(100.f + lane * laneWidth, startY));
    }

    void update(float dt) {
        shape.move(Vector2f(0, NOTE_SPEED * dt));
        if (shape.getPosition().y > 1080)
            active = false;
    }
};

int main() {
    RenderWindow window(VideoMode(Vector2u(1920, 1080)), "Boss Fight - Sir Łukasz", State::Fullscreen);
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(0)));

    Texture bgTex, bossTex;
    if (!bgTex.loadFromFile("background.png") || !bossTex.loadFromFile("boss.png")) {
        cout << "Blad: nie udalo sie zaladowac tekstur.\n";

    }

    Sprite background(bgTex), boss(bossTex);
    boss.setPosition(Vector2f(700.f, 100.f));

    Font font;
    if (!font.openFromFile("txtures/fonts/font1.otf")) {
        cout << "Blad: nie udalo sie zaladowac czcionki.\n";
    }

    Text scoreText(font, "Score: 0",  32);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(Vector2f(20.f, 20.f));

    RectangleShape targetBar(Vector2f(160.f * NUM_LANES, 10.f));
    targetBar.setFillColor(Color::White);
    targetBar.setPosition(Vector2f(100.f, TARGET_Y + NOTE_SIZE.y));

    vector<Note> notes;
    Clock clock;
    float spawnTimer = 0.f;
    int score = 0;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        spawnTimer += dt;
        if (spawnTimer >= SPAWN_INTERVAL) {
            spawnTimer = 0.f;
            int lane = rand() % NUM_LANES;
            notes.emplace_back(lane, -NOTE_SIZE.y);
        }

        for (auto& n : notes)
            n.update(dt);

        for (int lane = 0; lane < NUM_LANES; lane++) {
            if (Keyboard::isKeyPressed(laneKeys[lane])) {
                for (auto& note : notes) {
                    if (note.lane == lane && note.active) {
                        float yDist = abs(note.shape.getPosition().y - TARGET_Y);
                        if (yDist <= HIT_WINDOW) {
                            note.active = false;
                            score += 100;
                            break;
                        }
                    }
                }
            }
        }

        notes.erase(remove_if(notes.begin(), notes.end(), [](Note& n) { return !n.active; }), notes.end());

        scoreText.setString("Score: " + to_string(score));

        window.clear();
        window.draw(background);
        window.draw(boss);
        window.draw(targetBar);
        for (auto& n : notes)
            window.draw(n.shape);
        window.draw(scoreText);
        window.display();
    }

    return 0;
}
