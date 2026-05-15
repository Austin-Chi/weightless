#include "light_factory.h"

MTL::Buffer* LightFactory::buildLights(MTL::Device* device, simd::float4* positionRadii, simd::float4* colorIntensities, NS::UInteger lightCount)
{

    Light* lights = new Light[lightCount];
    for (int i = 0; i < lightCount; i++) {
        lights[i].setPositionRadius(positionRadii[i]);
        lights[i].setColorIntensity(colorIntensities[i]);
    }
    NS::UInteger lightBufferSize = lightCount * sizeof(Light);
    MTL::Buffer* lightBuffer = device->newBuffer(lightBufferSize, MTL::ResourceStorageModeShared);
    memcpy(lightBuffer->contents(), lights, lightBufferSize);
    delete[] lights;

    return lightBuffer;
}