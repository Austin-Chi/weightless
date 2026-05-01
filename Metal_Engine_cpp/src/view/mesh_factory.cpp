//
//  mesh_factory.cpp
//  Metal_Engine_cpp
//
//  Created by 錡亭勳 on 4/22/26.
//

#include "mesh_factory.h"
#include <stb_image.h>
#include <iostream>
#include "vertex_formats.h"
#include "../backend/mtlm.h"

MTL::Buffer* MeshFactory::buildTriangle(MTL::Device *device)
{
    
    Vertex vertices[3] = {
        {(float[6]){-0.75, -0.75, -10.0, 1.0, 1.0, 0.0}},
        {(float[6]){ 0.75, -0.75, -10.0, 1.0, 1.0, 0.0}},
        {(float[6]){  0.0,  0.75, -10.0, 1.0, 1.0, 0.0}}
    };
    
    MTL::Buffer* buffer = device->newBuffer(3 * sizeof(Vertex), MTL::ResourceStorageModeShared);
    
    memcpy(buffer->contents(), vertices, 3 * sizeof(Vertex));
    
    return buffer;
}

Mesh MeshFactory::buildQuad(MTL::Device* device, const char* textureFileName)
{
    Mesh mesh;
    
    TexturedVertex vertices[4] = {
        {(float[8]){-0.75, -0.75, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0}},
        {(float[8]){ 0.75, -0.75, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0}},
        {(float[8]){ 0.75,  0.75, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0}},
        {(float[8]){-0.75,  0.75, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0}}
    };
    NS::UInteger vertexBufferSize = 4 * sizeof(TexturedVertex);
    
    ushort indices[6] = {0, 1, 2, 2, 3, 0};
    NS::UInteger indexBufferSize = 6 * sizeof(ushort);
    
    //vertex buffer
    MTL::Buffer* vertexBuffer = device->newBuffer(vertexBufferSize, MTL::ResourceStorageModeShared);
    memcpy(vertexBuffer->contents(), vertices, vertexBufferSize);
    mesh.setVertexBuffer(vertexBuffer);
    
    //index buffer
    MTL::Buffer* indexBuffer = device->newBuffer(indexBufferSize, MTL::ResourceStorageModeShared);
    memcpy(indexBuffer->contents(), indices, indexBufferSize);
    mesh.setIndexCount(6);
    mesh.setIndexBuffer(indexBuffer);
  
    //Load Texture
    int texWidth, texHeight, texChannels;
    unsigned char* textureData = stbi_load(textureFileName, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!textureData) {
        std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
        return mesh;
    }

    MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
    textureDescriptor->setWidth(texWidth);
    textureDescriptor->setHeight(texHeight);
    textureDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatRGBA8Unorm);
    textureDescriptor->setTextureType(MTL::TextureType2D);
    textureDescriptor->setMipmapLevelCount(1);
    textureDescriptor->setSampleCount(1);
    textureDescriptor->setStorageMode(MTL::StorageModeShared);
    textureDescriptor->setUsage(MTL::TextureUsageShaderRead);
    textureDescriptor->setAllowGPUOptimizedContents(true);
    MTL::Texture* texture = device->newTexture(textureDescriptor);
    MTL::Region copyRegion = MTL::Region::Make2D(0, 0, texWidth, texHeight);
    texture->replaceRegion(copyRegion, 0, textureData, 4 * texWidth);
    mesh.setTexture(texture);

    MTL::SamplerDescriptor* samplerDescriptor = MTL::SamplerDescriptor::alloc()->init();
    samplerDescriptor->setMinFilter(MTL::SamplerMinMagFilter::SamplerMinMagFilterLinear);
    samplerDescriptor->setMagFilter(MTL::SamplerMinMagFilter::SamplerMinMagFilterLinear);
    samplerDescriptor->setMipFilter(MTL::SamplerMipFilter::SamplerMipFilterNotMipmapped);
    samplerDescriptor->setMaxAnisotropy(1);
    samplerDescriptor->setSAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
    samplerDescriptor->setTAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
    samplerDescriptor->setRAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
    mesh.setSampler(device->newSamplerState(samplerDescriptor));

    stbi_image_free(textureData);

    return mesh;
}

Mesh MeshFactory::buildVoxel(MTL::Device* device, const char* textureFileName)
{
    Mesh mesh;
    
    TexturedVertex vertices[24] = { //each face has 4 vertices, and each vertex has 8 floats (pos(3), color(3), texCoord(2))
        //face -z
        {(float[8]){ 0.0, 0.0, -2.0, 1.0, 0.0, 0.0, 0.0, 0.0}},
        {(float[8]){ 0.0, 1.0, -2.0, 0.0, 1.0, 0.0, 1.0, 0.0}},
        {(float[8]){ 1.0, 1.0, -2.0, 0.0, 0.0, 1.0, 1.0, 1.0}},
        {(float[8]){ 1.0, 0.0, -2.0, 0.0, 1.0, 0.0, 0.0, 1.0}},
        //face +z
        {(float[8]){ 0.0, 0.0, -1.0, 1.0, 0.0, 0.0, 0.0, 0.0}},
        {(float[8]){ 1.0, 0.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0}},
        {(float[8]){ 1.0, 1.0, -1.0, 0.0, 0.0, 1.0, 1.0, 1.0}},
        {(float[8]){ 0.0, 1.0, -1.0, 0.0, 1.0, 0.0, 0.0, 1.0}},
        //face +x
        {(float[8]){ 1.0, 0.0, -2.0, 1.0, 0.0, 0.0, 0.0, 0.0}},
        {(float[8]){ 1.0, 1.0, -2.0, 0.0, 1.0, 0.0, 1.0, 0.0}},
        {(float[8]){ 1.0, 1.0, -1.0, 0.0, 0.0, 1.0, 1.0, 1.0}},
        {(float[8]){ 1.0, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 1.0}},
        //face -x
        {(float[8]){ 0.0, 1.0, -2.0, 1.0, 0.0, 0.0, 0.0, 0.0}},
        {(float[8]){ 0.0, 0.0, -2.0, 0.0, 1.0, 0.0, 1.0, 0.0}},
        {(float[8]){ 0.0, 0.0, -1.0, 0.0, 0.0, 1.0, 1.0, 1.0}},
        {(float[8]){ 0.0, 1.0, -1.0, 0.0, 1.0, 0.0, 0.0, 1.0}},
        //face +y
        {(float[8]){ 1.0, 1.0, -2.0, 1.0, 0.0, 0.0, 0.0, 0.0}},
        {(float[8]){ 0.0, 1.0, -2.0, 0.0, 1.0, 0.0, 1.0, 0.0}},
        {(float[8]){ 0.0, 1.0, -1.0, 0.0, 0.0, 1.0, 1.0, 1.0}},
        {(float[8]){ 1.0, 1.0, -1.0, 0.0, 1.0, 0.0, 0.0, 1.0}},
        //face -y
        {(float[8]){ 0.0, 0.0, -2.0, 1.0, 0.0, 0.0, 0.0, 0.0}},
        {(float[8]){ 1.0, 0.0, -2.0, 0.0, 1.0, 0.0, 1.0, 0.0}},
        {(float[8]){ 1.0, 0.0, -1.0, 0.0, 0.0, 1.0, 1.0, 1.0}},
        {(float[8]){ 0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 1.0}}
    };
    NS::UInteger vertexBufferSize = 24 * sizeof(TexturedVertex);
    
    ushort indices[36] = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};
    NS::UInteger indexBufferSize = 36 * sizeof(ushort);
    
    //vertex buffer
    MTL::Buffer* vertexBuffer = device->newBuffer(vertexBufferSize, MTL::ResourceStorageModeShared);
    memcpy(vertexBuffer->contents(), vertices, vertexBufferSize);
    mesh.setVertexBuffer(vertexBuffer);
    
    //index buffer
    MTL::Buffer* indexBuffer = device->newBuffer(indexBufferSize, MTL::ResourceStorageModeShared);
    memcpy(indexBuffer->contents(), indices, indexBufferSize);
    mesh.setIndexCount(36);
    mesh.setIndexBuffer(indexBuffer);
  
    //Load Texture
    int texWidth, texHeight, texChannels;
    unsigned char* textureData = stbi_load(textureFileName, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!textureData) {
        std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
        return mesh;
    }

    MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
    textureDescriptor->setWidth(texWidth);
    textureDescriptor->setHeight(texHeight);
    textureDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatRGBA8Unorm);
    textureDescriptor->setTextureType(MTL::TextureType2D);
    textureDescriptor->setMipmapLevelCount(1);
    textureDescriptor->setSampleCount(1);
    textureDescriptor->setStorageMode(MTL::StorageModeShared);
    textureDescriptor->setUsage(MTL::TextureUsageShaderRead);
    textureDescriptor->setAllowGPUOptimizedContents(true);
    MTL::Texture* texture = device->newTexture(textureDescriptor);
    MTL::Region copyRegion = MTL::Region::Make2D(0, 0, texWidth, texHeight);
    texture->replaceRegion(copyRegion, 0, textureData, 4 * texWidth);
    mesh.setTexture(texture);

    MTL::SamplerDescriptor* samplerDescriptor = MTL::SamplerDescriptor::alloc()->init();
    samplerDescriptor->setMinFilter(MTL::SamplerMinMagFilter::SamplerMinMagFilterLinear);
    samplerDescriptor->setMagFilter(MTL::SamplerMinMagFilter::SamplerMinMagFilterLinear);
    samplerDescriptor->setMipFilter(MTL::SamplerMipFilter::SamplerMipFilterLinear);
    samplerDescriptor->setMaxAnisotropy(4);
    samplerDescriptor->setSAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
    samplerDescriptor->setTAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
    samplerDescriptor->setRAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
    mesh.setSampler(device->newSamplerState(samplerDescriptor));

    stbi_image_free(textureData);

    return mesh;
}


Mesh MeshFactory::buildVoxelChunkMesh(MTL::Device* device, const char* textureFileName, bool* voxelLayout, int lengthX, int lengthY, int lengthZ, int blockCount)
{
    Mesh mesh;

    // calculate visible faces based on voxel layout
    int visibleFaceCount = 0;
    for(int z = 0; z < lengthZ; z++) {
        for(int y = 0; y < lengthY; y++) {
            for(int x = 0; x < lengthX; x++) {
                if(voxelLayout[x + y * lengthX + z * lengthX * lengthY]) {
                    // check neighbors to determine visible faces
                    if(z == 0 || !voxelLayout[x + y * lengthX + (z - 1) * lengthX * lengthY]) visibleFaceCount++; // -z face
                    if(z == lengthZ - 1 || !voxelLayout[x + y * lengthX + (z + 1) * lengthX * lengthY]) visibleFaceCount++; // +z face
                    if(x == lengthX - 1 || !voxelLayout[(x + 1) + y * lengthX + z * lengthX * lengthY]) visibleFaceCount++; // +x face
                    if(x == 0 || !voxelLayout[(x - 1) + y * lengthX + z * lengthX * lengthY]) visibleFaceCount++; // -x face
                    if(y == lengthY - 1 || !voxelLayout[x + (y + 1) * lengthX + z * lengthX * lengthY]) visibleFaceCount++; // +y face
                    if(y == 0 || !voxelLayout[x + (y - 1) * lengthX + z * lengthX * lengthY]) visibleFaceCount++; // -y face
                }
            }
        }
    }

    TexturedVertex* visibleVertices = new TexturedVertex[visibleFaceCount * 4];
    ushort* visibleIndices = new ushort[visibleFaceCount * 6];

    NS::UInteger vertexBufferSize = visibleFaceCount * 4 * sizeof(TexturedVertex);

    NS::UInteger indexBufferSize = visibleFaceCount * 6 * sizeof(ushort);
    
    //set the visible vertices and indices based on the voxel layout
    int currentFace = 0;
    for (int z = 0; z < lengthZ; z++) {
        for (int y = 0; y < lengthY; y++) {
            for (int x = 0; x < lengthX; x++) {
                if (voxelLayout[x + y * lengthX + z * lengthX * lengthY]) {

                    // -z face
                    if (z == 0 || !voxelLayout[x + y * lengthX + (z - 1) * lengthX * lengthY]) {
                        int vertexStartIndex = currentFace * 4;
                        // add vertices and indices for -z face
                        visibleVertices[vertexStartIndex] = TexturedVertex((float[8]){(float)x, (float)y, (float)z, 1.0, 0.0, 0.0, 0.0, 0.0});
                        visibleVertices[vertexStartIndex + 1] = TexturedVertex((float[8]){(float)x, (float)(y + 1), (float)z, 0.0, 1.0, 0.0, 1.0, 0.0});
                        visibleVertices[vertexStartIndex + 2] = TexturedVertex((float[8]){(float)(x + 1), (float)(y + 1), (float)z, 0.0, 0.0, 1.0, 1.0, 1.0});
                        visibleVertices[vertexStartIndex + 3] = TexturedVertex((float[8]){(float)(x + 1), (float)y, (float)z, 0.0, 1.0, 0.0, 0.0, 1.0});
                        int indexStartIndex = currentFace * 6;
                        visibleIndices[indexStartIndex] = vertexStartIndex;
                        visibleIndices[indexStartIndex + 1] = vertexStartIndex + 1;
                        visibleIndices[indexStartIndex + 2] = vertexStartIndex + 2;
                        visibleIndices[indexStartIndex + 3] = vertexStartIndex + 2;
                        visibleIndices[indexStartIndex + 4] = vertexStartIndex + 3;
                        visibleIndices[indexStartIndex + 5] = vertexStartIndex;
                        currentFace++;
                    }
                    // +z face
                    if (z == lengthZ - 1 || !voxelLayout[x + y * lengthX + (z + 1) * lengthX * lengthY]) {

                        int vertexStartIndex = currentFace * 4;
                        // add vertices and indices for +z face
                        visibleVertices[vertexStartIndex] = TexturedVertex((float[8]){(float)x, (float)(y + 1), (float)(z + 1), 1.0, 0.0, 0.0, 0.0, 0.0});
                        visibleVertices[vertexStartIndex + 1] = TexturedVertex((float[8]){(float)x, (float)y, (float)(z + 1), 0.0, 1.0, 0.0, 1.0, 0.0});
                        visibleVertices[vertexStartIndex + 2] = TexturedVertex((float[8]){(float)(x + 1), (float)y, (float)(z + 1), 0.0, 0.0, 1.0, 1.0, 1.0});
                        visibleVertices[vertexStartIndex + 3] = TexturedVertex((float[8]){(float)(x + 1), (float)(y + 1), (float)(z + 1), 0.0, 1.0, 0.0, 0.0, 1.0});
                        int indexStartIndex = currentFace * 6;
                        visibleIndices[indexStartIndex] = vertexStartIndex;
                        visibleIndices[indexStartIndex + 1] = vertexStartIndex + 1;
                        visibleIndices[indexStartIndex + 2] = vertexStartIndex + 2;
                        visibleIndices[indexStartIndex + 3] = vertexStartIndex + 2;
                        visibleIndices[indexStartIndex + 4] = vertexStartIndex + 3;
                        visibleIndices[indexStartIndex + 5] = vertexStartIndex;
                        currentFace++;
                    }
                    // -y face
                    if (y == 0 || !voxelLayout[x + (y - 1) * lengthX + z * lengthX * lengthY]) {

                        int vertexStartIndex = currentFace * 4;
                        // add vertices and indices for -y face
                        visibleVertices[vertexStartIndex] = TexturedVertex((float[8]){(float)x, (float)y, (float)(z + 1), 1.0, 0.0, 0.0, 0.0, 0.0});
                        visibleVertices[vertexStartIndex + 1] = TexturedVertex((float[8]){(float)x, (float)y, (float)z, 0.0, 1.0, 0.0, 1.0, 0.0});
                        visibleVertices[vertexStartIndex + 2] = TexturedVertex((float[8]){(float)(x + 1), (float)y, (float)z, 0.0, 0.0, 1.0, 1.0, 1.0});
                        visibleVertices[vertexStartIndex + 3] = TexturedVertex((float[8]){(float)(x + 1), (float)y, (float)(z + 1), 0.0, 1.0, 0.0, 0.0, 1.0});
                        int indexStartIndex = currentFace * 6;
                        visibleIndices[indexStartIndex] = vertexStartIndex;
                        visibleIndices[indexStartIndex + 1] = vertexStartIndex + 1;
                        visibleIndices[indexStartIndex + 2] = vertexStartIndex + 2;
                        visibleIndices[indexStartIndex + 3] = vertexStartIndex + 2;
                        visibleIndices[indexStartIndex + 4] = vertexStartIndex + 3;
                        visibleIndices[indexStartIndex + 5] = vertexStartIndex;
                        currentFace++;
                    }
                    // +y face
                    if (y == lengthY - 1 || !voxelLayout[x + (y + 1) * lengthX + z * lengthX * lengthY]) {
                        int vertexStartIndex = currentFace * 4;
                        // add vertices and indices for +y face
                        visibleVertices[vertexStartIndex] = TexturedVertex((float[8]){(float)(x + 1), (float)(y + 1), (float)(z + 1), 1.0, 0.0, 0.0, 0.0, 0.0});
                        visibleVertices[vertexStartIndex + 1] = TexturedVertex((float[8]){(float)(x + 1), (float)(y + 1), (float)z, 0.0, 1.0, 0.0, 1.0, 0.0});
                        visibleVertices[vertexStartIndex + 2] = TexturedVertex((float[8]){(float)x, (float)(y + 1), (float)z, 0.0, 0.0, 1.0, 1.0, 1.0});
                        visibleVertices[vertexStartIndex + 3] = TexturedVertex((float[8]){(float)x, (float)(y + 1), (float)(z + 1), 0.0, 1.0, 0.0, 0.0, 1.0});
                        int indexStartIndex = currentFace * 6;
                        visibleIndices[indexStartIndex] = vertexStartIndex;
                        visibleIndices[indexStartIndex + 1] = vertexStartIndex + 1;
                        visibleIndices[indexStartIndex + 2] = vertexStartIndex + 2;
                        visibleIndices[indexStartIndex + 3] = vertexStartIndex + 2;
                        visibleIndices[indexStartIndex + 4] = vertexStartIndex + 3;
                        visibleIndices[indexStartIndex + 5] = vertexStartIndex;
                        currentFace++;
                    }
                    // -x face
                    if (x == 0 || !voxelLayout[x - 1 + y * lengthX + z * lengthX * lengthY]) {
                        int vertexStartIndex = currentFace * 4;
                        // add vertices and indices for -x face
                        visibleVertices[vertexStartIndex] = TexturedVertex((float[8]){(float)x, (float)(y + 1), (float)(z + 1), 1.0, 0.0, 0.0, 0.0, 0.0});
                        visibleVertices[vertexStartIndex + 1] = TexturedVertex((float[8]){(float)x, (float)(y + 1), (float)z, 0.0, 1.0, 0.0, 1.0, 0.0});
                        visibleVertices[vertexStartIndex + 2] = TexturedVertex((float[8]){(float)x, (float)y, (float)z, 0.0, 0.0, 1.0, 1.0, 1.0});
                        visibleVertices[vertexStartIndex + 3] = TexturedVertex((float[8]){(float)x, (float)y, (float)(z + 1), 0.0, 1.0, 0.0, 0.0, 1.0});
                        int indexStartIndex = currentFace * 6;
                        visibleIndices[indexStartIndex] = vertexStartIndex;
                        visibleIndices[indexStartIndex + 1] = vertexStartIndex + 1;
                        visibleIndices[indexStartIndex + 2] = vertexStartIndex + 2;
                        visibleIndices[indexStartIndex + 3] = vertexStartIndex + 2;
                        visibleIndices[indexStartIndex + 4] = vertexStartIndex + 3;
                        visibleIndices[indexStartIndex + 5] = vertexStartIndex;
                        currentFace++;
                    }
                    // +x face
                    if (x == lengthX - 1 || !voxelLayout[x + 1 + y * lengthX + z * lengthX * lengthY]) {
                        int vertexStartIndex = currentFace * 4;
                        // add vertices and indices for +x face
                        visibleVertices[vertexStartIndex] = TexturedVertex((float[8]){(float)(x + 1), (float)y, (float)(z + 1), 1.0, 0.0, 0.0, 0.0, 0.0});
                        visibleVertices[vertexStartIndex + 1] = TexturedVertex((float[8]){(float)(x + 1), (float)y, (float)z, 0.0, 1.0, 0.0, 1.0, 0.0});
                        visibleVertices[vertexStartIndex + 2] = TexturedVertex((float[8]){(float)(x + 1), (float)(y + 1), (float)z, 0.0, 0.0, 1.0, 1.0, 1.0});
                        visibleVertices[vertexStartIndex + 3] = TexturedVertex((float[8]){(float)(x + 1), (float)(y + 1), (float)(z + 1), 0.0, 1.0, 0.0, 0.0, 1.0});
                        int indexStartIndex = currentFace * 6;
                        visibleIndices[indexStartIndex] = vertexStartIndex;
                        visibleIndices[indexStartIndex + 1] = vertexStartIndex + 1;
                        visibleIndices[indexStartIndex + 2] = vertexStartIndex + 2;
                        visibleIndices[indexStartIndex + 3] = vertexStartIndex + 2;
                        visibleIndices[indexStartIndex + 4] = vertexStartIndex + 3;
                        visibleIndices[indexStartIndex + 5] = vertexStartIndex;
                        currentFace++;
                    }                 

                }
            }
        }
    }
    //vertex buffer
    MTL::Buffer* vertexBuffer = device->newBuffer(vertexBufferSize, MTL::ResourceStorageModeShared);
    memcpy(vertexBuffer->contents(), visibleVertices, vertexBufferSize);
    mesh.setVertexBuffer(vertexBuffer);

    //instance buffer
    NS::UInteger instanceBufferSize = sizeof(simd::float4x4);
    simd::float4x4 instanceData = mtlm::identity();
    MTL::Buffer* instanceBuffer = device->newBuffer(instanceBufferSize, MTL::ResourceStorageModeShared);
    memcpy(instanceBuffer->contents(), &instanceData, instanceBufferSize);
    mesh.setInstanceBuffer(instanceBuffer);
    
    //index buffer
    MTL::Buffer* indexBuffer = device->newBuffer(indexBufferSize, MTL::ResourceStorageModeShared);
    memcpy(indexBuffer->contents(), visibleIndices, indexBufferSize);
    mesh.setIndexCount(visibleFaceCount * 6);
    mesh.setIndexBuffer(indexBuffer);
  
    //Load Texture
    int texWidth, texHeight, texChannels;
    unsigned char* textureData = stbi_load(textureFileName, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!textureData) {
        std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
        return mesh;
    }

    MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
    textureDescriptor->setWidth(texWidth);
    textureDescriptor->setHeight(texHeight);
    textureDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatRGBA8Unorm);
    textureDescriptor->setTextureType(MTL::TextureType2D);
    textureDescriptor->setMipmapLevelCount(1);
    textureDescriptor->setSampleCount(1);
    textureDescriptor->setStorageMode(MTL::StorageModeShared);
    textureDescriptor->setUsage(MTL::TextureUsageShaderRead);
    textureDescriptor->setAllowGPUOptimizedContents(true);
    MTL::Texture* texture = device->newTexture(textureDescriptor);
    MTL::Region copyRegion = MTL::Region::Make2D(0, 0, texWidth, texHeight);
    texture->replaceRegion(copyRegion, 0, textureData, 4 * texWidth);
    mesh.setTexture(texture);

    MTL::SamplerDescriptor* samplerDescriptor = MTL::SamplerDescriptor::alloc()->init();
    samplerDescriptor->setMinFilter(MTL::SamplerMinMagFilter::SamplerMinMagFilterLinear);
    samplerDescriptor->setMagFilter(MTL::SamplerMinMagFilter::SamplerMinMagFilterLinear);
    samplerDescriptor->setMipFilter(MTL::SamplerMipFilter::SamplerMipFilterLinear);
    samplerDescriptor->setMaxAnisotropy(4);
    samplerDescriptor->setSAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
    samplerDescriptor->setTAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
    samplerDescriptor->setRAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
    mesh.setSampler(device->newSamplerState(samplerDescriptor));

    stbi_image_free(textureData);

    return mesh;
}
