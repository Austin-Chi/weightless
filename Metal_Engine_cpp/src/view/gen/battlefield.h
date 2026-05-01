#pragma once
#include "../../config.h"
#include "../mesh_factory.h"
class Battlefield
{
public:
    Battlefield(int width, int height, int depth); // width: x-axis, height: y-axis, depth: z-axis
    ~Battlefield();
    void regenerate();
    bool* getBlockLayout();
    int getLengthX();
    int getLengthY();
    int getLengthZ();
    int getBlockCount();

private:
    int width=32, height=32, depth=32;
    int blockCount = 0;
    bool* blockLayout = nullptr; //1 for occupied, 0 for empty
    void generateBattlefield();
};