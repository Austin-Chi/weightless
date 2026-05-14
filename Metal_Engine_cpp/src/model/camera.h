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
    simd::float4 axisAngleToQuat(simd::float3 axis, float angle);
    simd::float4 quatMultiply(simd::float4 q1, simd::float4 q2);
    simd::float3 rotateByQuat(simd::float3 v, simd::float4 q);

    simd::float3 position = {0.0f, 0.0f, 5.0f};
    simd::float4 rotation = {0.0f, 0.0f, 0.0f, 1.0f};  // 四元數: x, y, z, w
    // 使用與原本 Euler 實作相同的基向量（前方為 X 正方向，右方為 Y 負方向）
    simd::float3 right = {0.0f, -1.0f, 0.0f};
    simd::float3 up = {0.0f, 0.0f, 1.0f};
    simd::float3 forwards = {1.0f, 0.0f, 0.0f};
};
