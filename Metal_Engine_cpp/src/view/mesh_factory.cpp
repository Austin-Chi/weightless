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

    //instance buffer
    NS::UInteger instanceBufferSize = sizeof(simd::float4x4);
    simd::float4x4 instanceData = mtlm::identity();
    MTL::Buffer* instanceBuffer = device->newBuffer(instanceBufferSize, MTL::ResourceStorageModeShared);
    memcpy(instanceBuffer->contents(), &instanceData, instanceBufferSize);
    mesh.setInstanceBuffer(instanceBuffer);
    
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

// OBJ loader merged into MeshFactory
Mesh MeshFactory::buildMeshFromOBJ(MTL::Device* device, const char* objFilePath)
{
    //provide a default texture for OBJ meshes that don't specify one, since the shader expects a texture to be bound
    return buildMeshFromOBJWithTexture(device, objFilePath, "Metal_Engine_cpp/assets/checkerBoard.jpg");
}

Mesh MeshFactory::buildMeshFromOBJWithTexture(MTL::Device* device, const char* objFilePath, const char* textureFileName)
{
    Mesh mesh;
    std::vector<simd::float3> positions;
    std::vector<simd::float3> normals;
    std::vector<simd::float2> texCoords;
    struct FaceVertex { int positionIndex; int texCoordIndex; int normalIndex; };
    std::vector<FaceVertex> faceVertices;

    auto parseFaceVertex = [](const std::string& faceVertexStr) {
        FaceVertex fv = {-1, -1, -1};
        std::stringstream ss(faceVertexStr);
        std::string part;
        int index = 0;
        while (std::getline(ss, part, '/')) {
            if (!part.empty()) {
                if (index == 0) fv.positionIndex = std::stoi(part) - 1;
                else if (index == 1) fv.texCoordIndex = std::stoi(part) - 1;
                else if (index == 2) fv.normalIndex = std::stoi(part) - 1;
            }
            index++;
        }
        return fv;
    };

    std::ifstream objFile(objFilePath);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open OBJ file: " << objFilePath << std::endl;
        return mesh;
    }

    std::string line;
    while (std::getline(objFile, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        line = line.substr(start);
        std::stringstream ss(line);
        std::string cmd; ss >> cmd;
        if (cmd == "v") {
            float x,y,z; ss >> x >> y >> z; positions.push_back(simd::make_float3(x,y,z));
        } else if (cmd == "vn") {
            float x,y,z; ss >> x >> y >> z; normals.push_back(simd::normalize(simd::make_float3(x,y,z)));
        } else if (cmd == "vt") {
            float u,v; ss >> u >> v; texCoords.push_back(simd::make_float2(u,v));
        } else if (cmd == "f") {
            std::vector<std::string> faceVertexStrs; std::string fvStr;
            while (ss >> fvStr) faceVertexStrs.push_back(fvStr);
            std::vector<FaceVertex> faceFVs; 
            for (const auto &s: faceVertexStrs) faceFVs.push_back(parseFaceVertex(s));
            if (faceFVs.size() == 3) {
                faceVertices.insert(faceVertices.end(), faceFVs.begin(), faceFVs.end());
            } else if (faceFVs.size() == 4) {
                faceVertices.push_back(faceFVs[0]); faceVertices.push_back(faceFVs[1]); faceVertices.push_back(faceFVs[2]);
                faceVertices.push_back(faceFVs[0]); faceVertices.push_back(faceFVs[2]); faceVertices.push_back(faceFVs[3]);
            } else {
                std::cerr << "Warning: Polygon with " << faceFVs.size() << " vertices encountered. Only triangles and quads are supported." << std::endl;
            }
        }
    }
    objFile.close();

    if (positions.empty() || faceVertices.empty()) {
        std::cerr << "OBJ file is empty or has no valid faces: " << objFilePath << std::endl;
        return mesh;
    }

    if (normals.empty()) {
        normals.resize(positions.size(), simd::make_float3(0.0f,0.0f,0.0f));
        for (size_t i = 0; i < faceVertices.size(); i += 3) {
            if (i+2 < faceVertices.size()) {
                int v0 = faceVertices[i].positionIndex;
                int v1 = faceVertices[i+1].positionIndex;
                int v2 = faceVertices[i+2].positionIndex;
                if (v0>=0 && v1>=0 && v2>=0 && v0 < (int)positions.size() && v1 < (int)positions.size() && v2 < (int)positions.size()) {
                    simd::float3 fn = simd::normalize(simd::cross(positions[v1]-positions[v0], positions[v2]-positions[v0]));
                    normals[v0] += fn; normals[v1] += fn; normals[v2] += fn;
                }
            }
        }
        for (auto &n: normals) n = simd::normalize(n);
    }

    std::vector<TexturedVertex> verticesOut;
    std::vector<uint32_t> indicesOut;
    verticesOut.reserve(faceVertices.size()); indicesOut.reserve(faceVertices.size());
    for (size_t i = 0; i < faceVertices.size(); ++i) {
        auto &fv = faceVertices[i];
        simd::float3 pos = (fv.positionIndex>=0 && fv.positionIndex < (int)positions.size()) ? positions[fv.positionIndex] : simd::make_float3(0,0,0);
        simd::float3 normal = (fv.normalIndex>=0 && fv.normalIndex < (int)normals.size()) ? normals[fv.normalIndex] : simd::make_float3(0,0,1);
        simd::float2 tex = (fv.texCoordIndex>=0 && fv.texCoordIndex < (int)texCoords.size()) ? texCoords[fv.texCoordIndex] : simd::make_float2(0,0);
        float data[8] = { pos[0], pos[1], pos[2], 1.0f,1.0f,1.0f, tex[0], tex[1] };
        verticesOut.push_back(TexturedVertex(data));
        indicesOut.push_back((uint32_t)i);
    }

    const bool useUInt32 = indicesOut.size() > 65535;
    if (useUInt32) mesh.setIndexType(MTL::IndexType::IndexTypeUInt32);

    NS::UInteger vertexBufferSize = verticesOut.size() * sizeof(TexturedVertex);
    MTL::Buffer* vertexBuffer = device->newBuffer(vertexBufferSize, MTL::ResourceStorageModeShared);
    memcpy(vertexBuffer->contents(), verticesOut.data(), vertexBufferSize);
    mesh.setVertexBuffer(vertexBuffer);

    NS::UInteger instanceBufferSize = sizeof(simd::float4x4);
    simd::float4x4 instanceData = mtlm::identity();
    MTL::Buffer* instanceBuffer = device->newBuffer(instanceBufferSize, MTL::ResourceStorageModeShared);
    memcpy(instanceBuffer->contents(), &instanceData, instanceBufferSize);
    mesh.setInstanceBuffer(instanceBuffer);

    MTL::Buffer* indexBuffer = nullptr;
    if (useUInt32) {
        NS::UInteger indexBufferSize = indicesOut.size() * sizeof(uint32_t);
        indexBuffer = device->newBuffer(indexBufferSize, MTL::ResourceStorageModeShared);
        memcpy(indexBuffer->contents(), indicesOut.data(), indexBufferSize);
    } else {
        std::vector<uint16_t> idx16; idx16.reserve(indicesOut.size());
        for (auto v: indicesOut) idx16.push_back(static_cast<uint16_t>(v));
        NS::UInteger indexBufferSize = idx16.size() * sizeof(uint16_t);
        indexBuffer = device->newBuffer(indexBufferSize, MTL::ResourceStorageModeShared);
        memcpy(indexBuffer->contents(), idx16.data(), indexBufferSize);
    }
    mesh.setIndexBuffer(indexBuffer);
    mesh.setIndexCount(indicesOut.size());

    if (textureFileName) {
        int texWidth, texHeight, texChannels;
        unsigned char* textureData = stbi_load(textureFileName, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        if (!textureData) {
            std::cout << "Warning: Failed to load texture: " << stbi_failure_reason() << std::endl;
        } else {
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
            MTL::Region copyRegion = MTL::Region::Make2D(0,0,texWidth,texHeight);
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
        }
    }

    std::cout << "Successfully loaded OBJ file: " << objFilePath << " (" << positions.size() << " vertices, " << indicesOut.size() << " indices)" << std::endl;
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

    //instance buffer
    NS::UInteger instanceBufferSize = sizeof(simd::float4x4);
    simd::float4x4 instanceData = mtlm::identity();
    MTL::Buffer* instanceBuffer = device->newBuffer(instanceBufferSize, MTL::ResourceStorageModeShared);
    memcpy(instanceBuffer->contents(), &instanceData, instanceBufferSize);
    mesh.setInstanceBuffer(instanceBuffer);
    
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


Mesh MeshFactory::buildVoxelChunkMesh(MTL::Device* device, const char* textureFileName, int* voxelLayout, int lengthX, int lengthY, int lengthZ, int blockCount)
{
    Mesh mesh;

    // calculate visible faces based on voxel layout
    int visibleFaceCount = 0;
    for(int z = 0; z < lengthZ; z++) {
        for(int y = 0; y < lengthY; y++) {
            for(int x = 0; x < lengthX; x++) {
                if(voxelLayout[x + y * lengthX + z * lengthX * lengthY]) {
                    // check neighbors to determine visible faces
                    if(z == 0 || voxelLayout[x + y * lengthX + (z - 1) * lengthX * lengthY] == 0) visibleFaceCount++; // -z face
                    if(z == lengthZ - 1 || voxelLayout[x + y * lengthX + (z + 1) * lengthX * lengthY] == 0) visibleFaceCount++; // +z face
                    if(x == lengthX - 1 || voxelLayout[(x + 1) + y * lengthX + z * lengthX * lengthY] == 0) visibleFaceCount++; // +x face
                    if(x == 0 || voxelLayout[(x - 1) + y * lengthX + z * lengthX * lengthY] == 0) visibleFaceCount++; // -x face
                    if(y == lengthY - 1 || voxelLayout[x + (y + 1) * lengthX + z * lengthX * lengthY] == 0) visibleFaceCount++; // +y face
                    if(y == 0 || voxelLayout[x + (y - 1) * lengthX + z * lengthX * lengthY] == 0) visibleFaceCount++; // -y face
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
                    if (z == 0 || voxelLayout[x + y * lengthX + (z - 1) * lengthX * lengthY] == 0) {
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
                    if (z == lengthZ - 1 || voxelLayout[x + y * lengthX + (z + 1) * lengthX * lengthY] == 0) {

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
                    if (y == 0 || voxelLayout[x + (y - 1) * lengthX + z * lengthX * lengthY] == 0) {

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
                    if (y == lengthY - 1 || voxelLayout[x + (y + 1) * lengthX + z * lengthX * lengthY] == 0) {
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
                    if (x == 0 || voxelLayout[x - 1 + y * lengthX + z * lengthX * lengthY] == 0) {
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
                    if (x == lengthX - 1 || voxelLayout[x + 1 + y * lengthX + z * lengthX * lengthY] == 0) {
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


