#include "battlefield.h"
#include "../mesh_factory.h"
#include "../../backend/mtlm.h"

Battlefield::Battlefield(int width, int height, int depth)
{
    this->width = width;
    this->height = height;
    this->depth = depth;
    blockLayout = new int[width * height * depth]();
    generateBattlefield();
}

Battlefield::~Battlefield()
{
    delete[] blockLayout;
    delete[] characterStartPositions;
}
int* Battlefield::getBlockLayout()
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

int* Battlefield::getCharacterStartPositions()
{
    return characterStartPositions;
}

void Battlefield::regenerate()
{
    generateBattlefield();
}

void Battlefield::generateBattlefield()
{
    blockCount = 0;
    // for (int i = 0; i < width * height * depth; ++i) {
    //     int x = i % width;
    //     int y = (i / width) % height;
    //     int z = i / (width * height);
    //     // just cover the outer shell of the battlefield
    //     if (x == 0 || x == width - 1 || y == 0 || y == height - 1 || z == 0 || z == depth - 1) {
    //         blockLayout[i] = true;
    //         blockCount++;
    //     }
    // }
    // create a layout by creating random (x, y, z, w, h, d) boxes and unifying them into the block layout
    bool* tempLayout = new bool[width * height * depth]();
    growBoxFrom(width/2, height/2, depth/2, width/4, width/2, tempLayout);
    expand(2, tempLayout, width/4);
    for (int i = 0; i < width * height * depth; i++) { 
        // if any of the neighbors of the corresponding block in tempLayout is true, set the block in blockLayout to true 
        int x = i % width; 
        int y = (i / width) % height; 
        int z = i / (width * height); 
        if (!tempLayout[i] && ((x > 0 && tempLayout[i - 1]) || (x < width - 1 && tempLayout[i + 1]) || (y > 0 && tempLayout[i - width]) || (y < height - 1 && tempLayout[i + width]) || (z > 0 && tempLayout[i - width * height]) || (z < depth - 1 && tempLayout[i + width * height]))) 
        { 
            blockLayout[i] = 1; 
            blockCount++; 
        } 
    }

    characterStartPositions[0] = width / 2; // x
    characterStartPositions[1] = height / 2; // y
    characterStartPositions[2] = depth / 2; // z
    characterStartPositions[3] = width / 2; // 2nd x
    characterStartPositions[4] = height / 2; // 2nd y
    characterStartPositions[5] = depth / 2; // 2nd z
    while (blockLayout[characterStartPositions[0] + characterStartPositions[1] * width + characterStartPositions[2] * width * height] == 0) {
        characterStartPositions[0]++;
    }
    while (blockLayout[characterStartPositions[3] + characterStartPositions[4] * width + characterStartPositions[5] * width * height] == 0) {
        characterStartPositions[3]--;
    }
    while (blockLayout[characterStartPositions[0] + characterStartPositions[1] * width + characterStartPositions[2] * width * height] == 0) {
        characterStartPositions[1]++;
    }
    while (blockLayout[characterStartPositions[3] + characterStartPositions[4] * width + characterStartPositions[5] * width * height] == 0) {
        characterStartPositions[4]--;
    }

}

bool Battlefield::isSurface(int x, int y, int z, bool* grid) {
    int idx = x + y * width + z * width * height;

    if (!grid[idx]) return false;

    if (x > 0 && !grid[idx - 1]) return true;
    if (x < width - 1 && !grid[idx + 1]) return true;
    if (y > 0 && !grid[idx - width]) return true;
    if (y < height - 1 && !grid[idx + width]) return true;
    if (z > 0 && !grid[idx - width * height]) return true;
    if (z < depth - 1 && !grid[idx + width * height]) return true;

    return false;
}

void Battlefield::growBoxFrom(int cx, int cy, int cz, int sizeMin, int sizeMax, bool* grid)
{
    int w = rand() % (sizeMax - sizeMin + 1) + sizeMin;
    int h = rand() % (sizeMax - sizeMin + 1) + sizeMin;
    int d = rand() % (sizeMax - sizeMin + 1) + sizeMin;

    int startX = std::clamp(cx - w/2, 1, width-1);
    int startY = std::clamp(cy - h/2, 1, height-1);
    int startZ = std::clamp(cz - d/2, 1, depth-1);

    for (int x = startX; x < std::min(startX + w, width - 1); x++)
    for (int y = startY; y < std::min(startY + h, height - 1); y++)
    for (int z = startZ; z < std::min(startZ + d, depth - 1); z++)
    {
        grid[x + y * width + z * width * height] = true;
    }
}

void Battlefield::expand(int depthLevel, bool* grid, int sizeMax)
{
    if (depthLevel == 0) return;

    std::vector<std::tuple<int,int,int>> frontier;

    // collect surface
    for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
    for (int z = 0; z < depth; z++)
        if (isSurface(x,y,z,grid))
            frontier.emplace_back(x,y,z);

    int numSeeds = frontier.size() / 4; // tune this

    for (int i = 0; i < numSeeds; i++)
    {
        auto [x,y,z] = frontier[rand() % frontier.size()];

        // probability gate (important for sparsity)
        // if (rand() % 100 < 60)
        growBoxFrom(x,y,z, sizeMax / 2, sizeMax, grid);
    }

    expand(depthLevel - 1, grid, sizeMax / 2);
}