#pragma once
#include "../../config.h"
#include "../mesh_factory.h"
class Battlefield
{
public:
    Battlefield(MTL::Device* device, NS::UInteger width, NS::UInteger height, NS::UInteger depth, Mesh* blockMesh); // width: x-axis, height: y-axis, depth: z-axis
    ~Battlefield();
    void regenerate();
private:
    MTL::Device* device;
    NS::UInteger width=32, height=32, depth=32;
    NS::UInteger blockCount = 0;
    bool* blockLayout = nullptr; //1 for occupied, 0 for empty
    MTL::Buffer* instanceBuffer = nullptr;
    Mesh* blockMesh = nullptr;
    void generateBattlefield();
};