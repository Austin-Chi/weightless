#pragma once
#include "../config.h"

class Mesh
{
public:
    void setVertexBuffer(MTL::Buffer* vertexBuffer);
    void setLightBuffer(MTL::Buffer* lightBuffer);
    void setLightCount(NS::UInteger lightCount);
    void setInstanceBuffer(MTL::Buffer* instanceBuffer);
    void setInstanceCount(NS::UInteger instanceCount);
    void setIndexCount(NS::UInteger indexCount);
    void setIndexType(MTL::IndexType indexType);
    void setIndexBuffer(MTL::Buffer* indexBuffer);
    void setTexture(MTL::Texture* texture);
    void setSampler(MTL::SamplerState* sampler);
    void draw(MTL::RenderCommandEncoder* encoder);
    void release();

private:
    MTL::Buffer* vertexBuffer = nullptr, *indexBuffer = nullptr, *instanceBuffer = nullptr, *lightBuffer = nullptr;
    MTL::Texture* texture = nullptr;
    MTL::SamplerState* sampler = nullptr;
    NS::UInteger indexCount = 0;
    NS::UInteger instanceCount = 1;
    NS::UInteger lightCount = 0;
    MTL::IndexType indexType = MTL::IndexType::IndexTypeUInt16;
};