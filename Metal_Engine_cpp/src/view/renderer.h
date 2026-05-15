//
//  renderer.h
//  Metal_Engine_cpp
//
//  Created by 錡亭勳 on 4/19/26.
//

#pragma once
#include "../config.h"
#include "mesh_factory.h"
#include "light_factory.h"
#include "gen/battlefield.h"

class Renderer
{
public:
    Renderer(MTL::Device* device, CA::MetalLayer* metalLayer, Battlefield* battlefield);
    ~Renderer();
    void update(const simd::float4x4& view);
    
private:
    void buildMeshes();
    void buildShaders();
    void buildDepthState();
    void buildLights();
    void ensureDepthTexture(NS::UInteger width, NS::UInteger height);
    MTL::Device* device;
    CA::MetalLayer* metalLayer;
    CA::MetalDrawable* drawableArea;
    MTL::CommandQueue* commandQueue;
    
    MTL::Buffer* triangleMesh;
    MTL::Buffer* lightsBuffer;
    MTL::RenderPipelineState* trianglePipeline, *generalPipeline;
    Mesh quadMesh, voxelMesh, battleFieldMesh, objMesh;
    MTL::Texture* depthTexture;
    MTL::DepthStencilState* depthState;
    Battlefield* battlefield;
    float t = 0.0f;
};
