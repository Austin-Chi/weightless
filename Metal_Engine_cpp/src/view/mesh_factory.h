//
//  mesh_factory.h
//  Metal_Engine_cpp
//
//  Created by 錡亭勳 on 4/22/26.
//

#pragma once
#include "../config.h"
#include "mesh.h"

namespace MeshFactory {
    MTL::Buffer* buildTriangle(MTL::Device* device);
    Mesh buildQuad(MTL::Device* device, const char* textureFileName);
    Mesh buildVoxel(MTL::Device* device, const char* textureFileName);
}
