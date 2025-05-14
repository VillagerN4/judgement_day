#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>
using namespace sf;
using namespace std;

const int NUM_LANES = 6;
float NOTE_SPEED = 550.f;
const float SPAWN_INTERVAL = 0.3f;
const float NOTE_RADIUS = 20.f;
const float TARGET_Y = 900.f;
const float HIT_WINDOW = 50.f;
const int MAX_NOTES = 1000;
const int MAX_MISSES = 20;

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

int main() {
    RenderWindow window(VideoMode(Vector2u(1920, 1080)), "Guitar hero", State::Fullscreen);
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(0)));

    Font font;
    if (!font.openFromFile("txtures/fonts/font1.otf")) return -1;

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

    vector<Note> notes;
    vector<Text> keyTexts;
    vector<CircleShape> hitMarkers;

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

    Clock clock;
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

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();



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
        window.display();
    }

    return 0;
}
