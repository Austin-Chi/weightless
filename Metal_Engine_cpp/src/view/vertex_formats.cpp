#include "vertex_formats.h"

Vertex::Vertex(float data[6])
{
    for (int i = 0; i < 6; ++i) {
        this->data[i] = data[i];
    }
}

MTL::VertexDescriptor* Vertex::getDescriptor()
{
    MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
    auto attributes = vertexDescriptor->attributes();
    NS::UInteger offset = 0;
    //attribute 0: pos (vec3)
    auto positionDescriptor = attributes->object(0);
    positionDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat3);
    positionDescriptor->setOffset(offset);
    positionDescriptor->setBufferIndex(0);
    offset += 3 * sizeof(float);
    //attribute 1: color (vec3)
    auto colorDescriptor = attributes->object(1);
    colorDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat3);
    colorDescriptor->setOffset(offset);
    colorDescriptor->setBufferIndex(0);
    offset += 3 * sizeof(float);
    
    auto layoutDescriptor = vertexDescriptor->layouts()->object(0);
    layoutDescriptor->setStride(offset);
    
    return vertexDescriptor;
}

TexturedVertex::TexturedVertex()
{
    for (int i = 0; i < 8; ++i) {
        this->data[i] = 0.0f;
    }
}

TexturedVertex::TexturedVertex(float data[8])
{
    for (int i = 0; i < 8; ++i) {
        this->data[i] = data[i];
    }
}

MTL::VertexDescriptor* TexturedVertex::getDescriptor()
{
    MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
    auto attributes = vertexDescriptor->attributes();
    NS::UInteger offset = 0;
    //attribute 0: pos (vec3)
    auto positionDescriptor = attributes->object(0);
    positionDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat3);
    positionDescriptor->setOffset(offset);
    positionDescriptor->setBufferIndex(0);
    offset += 3 * sizeof(float);
    //attribute 1: color (vec3)
    auto colorDescriptor = attributes->object(1);
    colorDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat3);
    colorDescriptor->setOffset(offset);
    colorDescriptor->setBufferIndex(0);
    offset += 3 * sizeof(float);
    //attribute 2: texCoord (vec2)
    auto texCoordDescriptor = attributes->object(2);
    texCoordDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat2);
    texCoordDescriptor->setOffset(offset);
    texCoordDescriptor->setBufferIndex(0);
    offset += 2 * sizeof(float);

    auto layoutDescriptor = vertexDescriptor->layouts()->object(0);
    layoutDescriptor->setStride(offset);

    return vertexDescriptor;
}
