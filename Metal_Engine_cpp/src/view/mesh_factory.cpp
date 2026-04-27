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
    samplerDescriptor->setMipFilter(MTL::SamplerMipFilter::SamplerMipFilterNotMipmapped);
    samplerDescriptor->setMaxAnisotropy(1);
    samplerDescriptor->setSAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
    samplerDescriptor->setTAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
    samplerDescriptor->setRAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
    mesh.setSampler(device->newSamplerState(samplerDescriptor));

    stbi_image_free(textureData);

    return mesh;
}
