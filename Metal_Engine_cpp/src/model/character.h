#pragma once
#include "../config.h"
#include "../view/mesh.h"
#include "../backend/mtlm.h"
#include "../view/gen/battlefield.h"

class Character
{
public:
    void setMesh(Mesh* mesh);
    void setPosition(simd::float3 pos);
    simd::float3 getPosition();
    simd::float3 getForwardVector();
    simd::float3 getUpVector();
    void setDirection(int forwardDirection, int upDirection);
    void counterclockwiseTurn();
    void draw(MTL::RenderCommandEncoder* encoder);
    void update(Battlefield* battlefield);
    void setMoving();

private:
    Mesh* mesh = nullptr;
    simd::float3 position = {0.0f, 0.0f, 0.0f};
    int forwardDirection = 0; // 0: +X, 1: +Y, 2: +Z, 3: -X, 4: -Y, 5: -Z
    int upDirection = 2; // 0: +X, 1: +Y, 2: +Z, 3: -X, 4: -Y, 5: -Z
    simd::float4x4 rotation = mtlm::identity();
    bool isMoving = false;
    float speed = 0.1f; // units per frame
};