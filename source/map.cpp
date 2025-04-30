#include "map.hpp"
#include <iostream>

using namespace std;

Map::Map(int grid_size_x, int grid_size_y, int grid_layers) : GRID_SIZE_X(grid_size_x), GRID_SIZE_Y(grid_size_y), GRID_LAYERS(grid_layers)
{}

void Map::Info(){
    cout << this -> GRID_SIZE_X << " | " << this -> GRID_SIZE_Y << " | " << this -> GRID_LAYERS << endl;
}


Map::~Map()
{}