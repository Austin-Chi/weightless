#pragma once
#include "../../config.h"
#include "../mesh_factory.h"
class Battlefield
{
public:
    Battlefield(int width, int height, int depth); // width: x-axis, height: y-axis, depth: z-axis
    ~Battlefield();
    void regenerate();
    int* getBlockLayout();
    int getLengthX();
    int getLengthY();
    int getLengthZ();
    int getBlockCount();
    int* getCharacterStartPositions();

private:
    int width=32, height=32, depth=32;
    int blockCount = 0;
    int* blockLayout = nullptr; //1 for occupied, 0 for empty
    int* characterStartPositions = new int[6](); // x, y, z
    void generateBattlefield();
    bool isSurface(int x, int y, int z, bool* grid);
    void growBoxFrom(int cx, int cy, int cz, int sizeMin, int sizeMax, bool* grid);
    void expand(int depthLevel, bool* grid, int sizeMax);
};