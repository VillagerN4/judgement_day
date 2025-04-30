#ifndef MAPGRID_H
#define MAPGRID_H

class Map
{
private:

    const int GRID_SIZE_X;
    const int GRID_SIZE_Y;
    const int GRID_LAYERS;

public:

    Map(int grid_size_x, int grid_size_y, int grid_layers);

    void Info();

    ~Map();
};


#endif