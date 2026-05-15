#pragma once
#include "../config.h"
#include "mesh.h"
#include "../model/light.h"

namespace LightFactory {
    MTL::Buffer* buildLights(MTL::Device* device, simd::float4* positionRadii, simd::float4* colorIntensities, NS::UInteger lightCount);
};