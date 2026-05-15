#pragma once
#include "../config.h"

class Light
{
public:
    void setPosition(simd::float3 pos);
    void setRadius(float radius);
    void setColor(simd::float3 color);
    void setIntensity(float intensity);
    void setPositionRadius(simd::float4 posRadius);
    void setColorIntensity(simd::float4 colorIntensity);
    simd::float4 getPositionRadius();
    simd::float4 getColorIntensity();
private:
    simd::float4 positionRadius = {0.0f, 0.0f, 0.0f, 1.0f};
    simd::float4 colorIntensity = {1.0f, 1.0f, 1.0f, 1.0f};
};