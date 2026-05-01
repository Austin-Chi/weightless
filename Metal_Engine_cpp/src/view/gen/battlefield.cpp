#include "battlefield.h"
#include "../mesh_factory.h"
#include "../../backend/mtlm.h"

Battlefield::Battlefield(int width, int height, int depth)
{
    this->width = width;
    this->height = height;
    this->depth = depth;
    blockLayout = new bool[width * height * depth]();
    generateBattlefield();
}

Battlefield::~Battlefield()
{
    delete[] blockLayout;
}
bool* Battlefield::getBlockLayout()
{
    return blockLayout;
}

int Battlefield::getLengthX()
{
    return width;
}

int Battlefield::getLengthY()
{
    return height;
}

int Battlefield::getLengthZ()
{
    return depth;
}

int Battlefield::getBlockCount()
{
    return blockCount;
}

void Battlefield::regenerate()
{
    generateBattlefield();
}

void Battlefield::generateBattlefield()
{
    blockCount = 0;
    for (int i = 0; i < width * height * depth; ++i) {
        int x = i % width;
        int y = (i / width) % height;
        int z = i / (width * height);
        // just cover the outer shell of the battlefield
        if (x == 0 || x == width - 1 || y == 0 || y == height - 1 || z == 0 || z == depth - 1) {
            blockLayout[i] = true;
            blockCount++;
        }
    }

}