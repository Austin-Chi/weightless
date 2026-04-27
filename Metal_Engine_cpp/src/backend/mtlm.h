//
//  mtlm.h
//  Metal_Engine_cpp
//
//  Created by 錡亭勳 on 4/22/26.
//

#pragma once
#include <simd/simd.h>

namespace mtlm
{
    simd::float4x4 identity();
    simd::float4x4 translation(simd::float3 dPos);
    simd::float4x4 z_rotation(float theta);
    simd::float4x4 scale(float factor);
    simd::float4x4 perspective_projection(float fovy, float aspect, float near, float far);
    simd::float4x4 view_matrix(simd::float3 right, simd::float3 up, simd::float3 forward, simd::float3 position);
}
