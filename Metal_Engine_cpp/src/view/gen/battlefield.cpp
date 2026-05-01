#include "battlefield.h"
#include "../mesh_factory.h"
#include "../../backend/mtlm.h"

Battlefield::Battlefield(MTL::Device* device, NS::UInteger width, NS::UInteger height, NS::UInteger depth, Mesh* blockMesh)
{
    this->device = device->retain();
    this->width = width;
    this->height = height;
    this->depth = depth;
    this->blockMesh = blockMesh;
    blockLayout = new bool[width * height * depth]();
    generateBattlefield();
}

Battlefield::~Battlefield()
{
    if (instanceBuffer) {
        instanceBuffer->release();
    }
    delete[] blockLayout;
    device->release();
}

void Battlefield::regenerate()
{
    generateBattlefield();
}

void Battlefield::generateBattlefield()
{
    blockCount = 0;
    for (NS::UInteger i = 0; i < width * height * depth; ++i) {
        int x = i % width;
        int y = (i / width) % height;
        int z = i / (width * height);
        // just cover the outer shell of the battlefield
        if (x == 0 || x == width - 1 || y == 0 || y == height - 1 || z == 0 || z == depth - 1) {
            blockLayout[i] = true;
            blockCount++;
        }
    }
    
    // if (instanceBuffer) {
    //     instanceBuffer->release();
    // }
    
    // simd::float4x4* instanceData = new simd::float4x4[blockCount];
    // NS::UInteger index = 0;
    // for (NS::UInteger z = 0; z < depth; ++z) {
    //     for (NS::UInteger y = 0; y < height; ++y) {
    //         for (NS::UInteger x = 0; x < width; ++x) {
    //             if (blockLayout[x + y * width + z * width * height]) {
    //                 instanceData[index++] = mtlm::translation({float(x), float(y), float(z)});
    //             }
    //         }
    //     }
    // }
    
    // NS::UInteger instanceBufferSize = blockCount * sizeof(simd::float4x4);
    // instanceBuffer = device->newBuffer(instanceData, instanceBufferSize, MTL::ResourceStorageModeShared);
    // memcpy(instanceBuffer->contents(), instanceData, instanceBufferSize);
    // blockMesh->setInstanceBuffer(instanceBuffer);
    // blockMesh->setInstanceCount(blockCount);
    // delete[] instanceData;
}