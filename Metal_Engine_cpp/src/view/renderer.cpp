//
//  renderer.cpp
//  Metal_Engine_cpp
//
//  Created by 錡亭勳 on 4/19/26.
//

#include "renderer.h"
#include "pipeline_factory.h"
#include "../backend/mtlm.h"
#include "vertex_formats.h"

Renderer::Renderer(MTL::Device* device, CA::MetalLayer* metalLayer, Battlefield* battlefield, Character* character):
device(device->retain()),
metalLayer(metalLayer->retain()),
commandQueue(device->newCommandQueue()->retain()),
battlefield(battlefield),
character(character)
{

    buildMeshes();
    buildShaders();
    buildDepthState();
    buildLights();
    character->setMesh(&objMesh);
}

Renderer::~Renderer()
{
    quadMesh.release();
    voxelMesh.release();
    objMesh.release();
    triangleMesh->release();
    lightsBuffer->release();
    if (trianglePipeline) {
        trianglePipeline->release();
    }
    if (generalPipeline) {
        generalPipeline->release();
    }
    if (depthState) {
        depthState->release();
    }
    if (depthTexture) {
        depthTexture->release();
    }
    commandQueue->release();
    device->release();
}

void Renderer::buildMeshes()
{
    triangleMesh = MeshFactory::buildTriangle(device);
    quadMesh = MeshFactory::buildQuad(device, "Metal_Engine_cpp/assets/checkerBoard.jpg");
    voxelMesh = MeshFactory::buildVoxel(device, "Metal_Engine_cpp/assets/checkerBoard.jpg");
    battleFieldMesh = MeshFactory::buildVoxelChunkMesh(device, "Metal_Engine_cpp/assets/checkerBoard.jpg", battlefield->getBlockLayout(), battlefield->getLengthX(), battlefield->getLengthY(), battlefield->getLengthZ(), battlefield->getBlockCount());
    
    // Load OBJ mesh
    objMesh = MeshFactory::buildMeshFromOBJ(device, "Metal_Engine_cpp/assets/mesh/cube.obj");
    std::cout << "Renderer debug: OBJ mesh loaded and queued for drawing" << std::endl;
}

void Renderer::buildShaders()
{
    PipelineBuilder* builder = new PipelineBuilder(device);
    
    const char* triangleShaderPath = "Metal_Engine_cpp/src/shaders/triangle.metal";
    const char* generalShaderPath = "Metal_Engine_cpp/src/shaders/general.metal";
    builder->set_vertex_descriptor(Vertex::getDescriptor());
    builder->set_filename(triangleShaderPath);
    builder->set_vertex_entry_point("vertexMain");
    builder->set_fragment_entry_point("fragmentMain");
    trianglePipeline = builder->build();
    
    builder->set_vertex_descriptor(TexturedVertex::getDescriptor());
    builder->set_filename(generalShaderPath);
    builder->set_vertex_entry_point("vertexMainGeneral");
    builder->set_fragment_entry_point("fragmentMainGeneral");
    generalPipeline = builder->build();
    
    delete builder;
}

void Renderer::buildDepthState()
{
    MTL::DepthStencilDescriptor* depthInfo = MTL::DepthStencilDescriptor::alloc()->init();
    depthInfo->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLess);
    depthInfo->setDepthWriteEnabled(true);

    depthState = device->newDepthStencilState(depthInfo);
    depthInfo->release();
}

void Renderer::ensureDepthTexture(NS::UInteger width, NS::UInteger height)
{
    if (width == 0 || height == 0) {
        return;
    }

    if (depthTexture && depthTexture->width() == width && depthTexture->height() == height) {
        return;
    }

    if (depthTexture) {
        depthTexture->release();
        depthTexture = nullptr;
    }

    MTL::TextureDescriptor* depthDescriptor = MTL::TextureDescriptor::alloc()->init();
    depthDescriptor->setTextureType(MTL::TextureType2D);
    depthDescriptor->setWidth(width);
    depthDescriptor->setHeight(height);
    depthDescriptor->setPixelFormat(MTL::PixelFormat::PixelFormatDepth32Float);
    depthDescriptor->setStorageMode(MTL::StorageModePrivate);
    depthDescriptor->setUsage(MTL::TextureUsageRenderTarget);
    depthTexture = device->newTexture(depthDescriptor);
    depthDescriptor->release();
}

void Renderer::buildLights()
{
    NS::UInteger lightCount = 1;
    simd::float4 positionRadii[1] = {{0.0f, 0.0f, 0.0f, 10.0f}};
    simd::float4 colorIntensities[1] = {{1.0f, 1.0f, 1.0f, 1.0f}};

    lightsBuffer = LightFactory::buildLights(device, positionRadii, colorIntensities, lightCount);

    //Bind light buffer to mesh
    quadMesh.setLightBuffer(lightsBuffer);
    quadMesh.setLightCount(lightCount);
    voxelMesh.setLightBuffer(lightsBuffer);
    voxelMesh.setLightCount(lightCount);
    battleFieldMesh.setLightBuffer(lightsBuffer);
    battleFieldMesh.setLightCount(lightCount);
    objMesh.setLightBuffer(lightsBuffer);
    objMesh.setLightCount(lightCount);
}

void Renderer::update(const simd::float4x4& view)
{
    t += 1.0f;
    if (t > 360) {
        t -= 360.0f;
    }
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
    
    MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
    MTL::RenderPassDescriptor* renderPass = MTL::RenderPassDescriptor::alloc()->init();
    
    drawableArea = metalLayer->nextDrawable();
    if (!drawableArea) {
        renderPass->release();
        pool->release();
        return;
    }

    ensureDepthTexture(drawableArea->texture()->width(), drawableArea->texture()->height());

    MTL::RenderPassColorAttachmentDescriptor* colorAttachment = renderPass->colorAttachments()->object(0);
    colorAttachment->setTexture(drawableArea->texture());
    colorAttachment->setLoadAction(MTL::LoadActionClear);
    colorAttachment->setClearColor(MTL::ClearColor(0.75f, 0.25f, 0.125f, 1.0f));
    colorAttachment->setStoreAction(MTL::StoreActionStore);

    MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = renderPass->depthAttachment();
    depthAttachment->setTexture(depthTexture);
    depthAttachment->setLoadAction(MTL::LoadActionClear);
    depthAttachment->setStoreAction(MTL::StoreActionDontCare);
    depthAttachment->setClearDepth(1.0f);
    
    MTL::RenderCommandEncoder* encoder = commandBuffer->renderCommandEncoder(renderPass);
    
    encoder->setRenderPipelineState(generalPipeline);
    encoder->setDepthStencilState(depthState);
    encoder->setCullMode(MTL::CullModeBack);
    encoder->setFrontFacingWinding(MTL::Winding::WindingCounterClockwise);

    simd::float4x4 perspective = mtlm::perspective_projection(45.0f, 4.0f/3.0f, 0.1f, 100.0f);
    encoder->setVertexBytes(&perspective, sizeof(simd::float4x4), 3);
    encoder->setVertexBytes(&view, sizeof(simd::float4x4), 4);
    simd::float4x4 transform = mtlm::translation({0.0f, 0.0f, 2.0f});
    encoder->setVertexBytes(&transform, sizeof(simd::float4x4), 2);
    // quadMesh.draw(encoder);
    // voxelMesh.draw(encoder);
    battleFieldMesh.draw(encoder);
    
    // Draw character mesh
    character->draw(encoder);
    
    transform = mtlm::translation({0.5f, 0.5f, 2.0f}) * mtlm::z_rotation(t) * mtlm::scale(0.1f);
    encoder->setVertexBytes(&transform, sizeof(simd::float4x4), 1);
    encoder->setRenderPipelineState(trianglePipeline);
    encoder->setVertexBuffer(triangleMesh, 0, 0);
    encoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
    
    encoder->endEncoding();
    commandBuffer->presentDrawable(drawableArea);
    commandBuffer->commit();

    renderPass->release();
    
    pool->release();
}
