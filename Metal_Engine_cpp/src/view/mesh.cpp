#include "mesh.h"

void Mesh::setVertexBuffer(MTL::Buffer* vertexBuffer)
{
    if (this->vertexBuffer) {
        this->vertexBuffer->release();
    }
    this->vertexBuffer = vertexBuffer->retain();
}

void Mesh::setInstanceBuffer(MTL::Buffer* instanceBuffer)
{
    if (this->instanceBuffer) {
        this->instanceBuffer->release();
    }
    this->instanceBuffer = instanceBuffer->retain();
}

void Mesh::setInstanceCount(NS::UInteger instanceCount)
{
    this->instanceCount = instanceCount;
}

void Mesh::setIndexCount(NS::UInteger indexCount)
{
    this->indexCount = indexCount;
}

void Mesh::setIndexBuffer(MTL::Buffer* indexBuffer)
{
    if (this->indexBuffer) {
        this->indexBuffer->release();
    }
    this->indexBuffer = indexBuffer->retain();
}

void Mesh::setTexture(MTL::Texture* texture)
{
    if (this->texture) {
        this->texture->release();
    }
    this->texture = texture->retain();
}

void Mesh::setSampler(MTL::SamplerState* sampler)
{
    if (this->sampler) {
        this->sampler->release();
    }
    this->sampler = sampler->retain();
}

void Mesh::draw(MTL::RenderCommandEncoder* encoder)
{
    encoder->setVertexBuffer(vertexBuffer, 0, 0);
    encoder->setVertexBuffer(instanceBuffer, 0, 1);
    encoder->setFragmentTexture(texture, 0);
    encoder->setFragmentSamplerState(sampler, 0);
    encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, indexCount, MTL::IndexType::IndexTypeUInt16, indexBuffer, NS::UInteger(0), instanceCount);

}

void Mesh::release()
{
    if (vertexBuffer) {
        vertexBuffer->release();
        vertexBuffer = nullptr;
    }
    if (indexBuffer) {
        indexBuffer->release();
        indexBuffer = nullptr;
    }
    if (texture) {
        texture->release();
        texture = nullptr;
    }
    if (sampler) {
        sampler->release();
        sampler = nullptr;
    }
}