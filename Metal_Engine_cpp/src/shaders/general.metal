//
//  general.metal
//  Metal_Engine_cpp
//
//  Created by 錡亭勳 on 4/22/26.
//

#include <metal_stdlib>
using namespace metal;

struct VertexInput {
    float3 position [[attribute(0)]];
    float3 color [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
    float3 normal [[attribute(3)]];
};

struct VertexOutput {
    float4 position [[position]];
    half3 color;
    float2 texCoord;
    float3 normal;
};

struct PointLight {
    float4 positionRadius; // xyz = position, w = radius
    float4 colorIntensity; // rgb = color, w = intensity
};

vertex VertexOutput vertexMainGeneral(
    VertexInput input [[stage_in]],
    constant float4x4* instanceTransform [[buffer(1)]],
    constant float4x4& transform [[buffer(2)]],
    constant float4x4& projection [[buffer(3)]],
    constant float4x4& view [[buffer(4)]],
    uint instanceID [[instance_id]]) {
    VertexOutput payload;
    half3 pos = half3(input.position);
    payload.position = float4(half4x4(projection) * half4x4(view) * half4x4(transform) * half4x4(instanceTransform[instanceID]) * half4(pos, 1.0));
    payload.color = half3(input.color);
    payload.texCoord = input.texCoord;
    payload.normal = input.normal;
    return payload;
}

fragment half4 fragmentMainGeneral(VertexOutput frag [[stage_in]],
    texture2d<half> material [[texture(0)]],
    sampler samplerObject [[sampler(0)]],
    constant PointLight* lights [[buffer(0)]],
    constant uint& lightCount [[buffer(1)]]) {
    half4 texColor = material.sample(samplerObject, frag.texCoord);
    float3 albedo = float3(texColor.rgb) * float3(frag.color);
    float3 normal = frag.normal;
    float3 finalColor = albedo * 0.05; // ambient term

    for (uint i = 0; i < lightCount; i++) {
        PointLight light = lights[i];
        float3 lightVec = light.positionRadius.xyz - frag.position.xyz;
        float lightRadius = light.positionRadius.w;
        float3 lightColor = light.colorIntensity.rgb;
        float lightIntensity = light.colorIntensity.w;
        float distance = length(lightVec);
        float3 lightDir = lightVec / max(distance, 0.001); // Normalize and prevent division by zero
        float diffuse = saturate(dot(normal, lightDir));

        // Attenuation based on distance
        float attenuation = 1.0; //saturate(1.0 - distance / lightRadius);
        finalColor += albedo * lightColor * diffuse * attenuation * lightIntensity;

    }
    return half4(half3(finalColor), texColor.a);
}
