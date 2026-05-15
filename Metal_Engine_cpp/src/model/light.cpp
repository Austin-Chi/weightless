#include "light.h"

void Light::setPosition(simd::float3 pos)
{
    this->positionRadius[0] = pos[0];
    this->positionRadius[1] = pos[1];
    this->positionRadius[2] = pos[2];
}

void Light::setRadius(float radius)
{
    this->positionRadius[3] = radius;
}

void Light::setPositionRadius(simd::float4 posRadius)
{
    this->positionRadius = posRadius;
}

void Light::setColor(simd::float3 color)
{
    this->colorIntensity[0] = color[0];
    this->colorIntensity[1] = color[1];
    this->colorIntensity[2] = color[2];
}

void Light::setIntensity(float intensity)
{
    this->colorIntensity[3] = intensity;
}

void Light::setColorIntensity(simd::float4 colorIntensity)
{
    this->colorIntensity = colorIntensity;
}

simd::float4 Light::getPositionRadius()
{
    return this->positionRadius;
}

simd::float4 Light::getColorIntensity()
{
    return this->colorIntensity;
}