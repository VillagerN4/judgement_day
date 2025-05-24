#ifndef WORLDMAP_H
#define WORLDMAP_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "map.hpp"

using namespace sf;
using namespace std;

void loadMap(const char* path);

void worldTick(float deltaTime);

void displayWorld(RenderWindow& window, Vector2u window_size);

#endif