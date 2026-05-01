//
//  renderer.h
//  Metal_Engine_cpp
//
//  Created by 錡亭勳 on 4/19/26.
//

#pragma once
#include "../config.h"
#include "mesh_factory.h"
#include "gen/battlefield.h"

class Renderer
{
public:
    Renderer(MTL::Device* device, CA::MetalLayer* metalLayer);
    ~Renderer();
    void update(const simd::float4x4& view);
    
private:
    void buildMeshes();
    void buildShaders();
    void buildDepthState();
    void ensureDepthTexture(NS::UInteger width, NS::UInteger height);
    void buildBattlefield();
    MTL::Device* device;
    CA::MetalLayer* metalLayer;
    CA::MetalDrawable* drawableArea;
    MTL::CommandQueue* commandQueue;
    
    MTL::Buffer* triangleMesh;
    MTL::RenderPipelineState* trianglePipeline, *generalPipeline;
    Mesh quadMesh, voxelMesh, battleFieldMesh;
    MTL::Texture* depthTexture;
    MTL::DepthStencilState* depthState;
    Battlefield* battlefield;
    float t = 0.0f;
};
