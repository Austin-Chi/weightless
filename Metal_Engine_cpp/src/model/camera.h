#pragma once
#include "../config.h"

class Camera
{
public:
    void setPosition(simd::float3 pos);
    void setAngles(float pitch, float yaw);
    void walk(simd::float3 dPos);
    void spin(float dPitch, float dYaw);
    simd::float4x4 getViewTransform();
private:
    void updateVectors();

    simd::float3 position = {0.0f, 0.0f, 5.0f};
    float pitch = 0.0f, yaw = 0.0f;
    simd::float3 right = {0.0f, -1.0f, 0.0f};
    simd::float3 up = {0.0f, 0.0f, 1.0f};
    simd::float3 forwards = {1.0f, 0.0f, 0.0f};
};