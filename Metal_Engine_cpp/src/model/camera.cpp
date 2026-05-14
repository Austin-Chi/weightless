#include "camera.h"
#include "../backend/mtlm.h"

void Camera::setPosition(simd::float3 pos)
{
    this->position = pos;
}

void Camera::setAngles(float pitch, float yaw)
{
    // 設定初始旋轉四元數
    rotation = {0.0f, 0.0f, 0.0f, 1.0f};
    
    float pitchRad = pitch * M_PI / 180.0f;
    float yawRad = yaw * M_PI / 180.0f;
    
    // 先繞 X 軸旋轉 (pitch)
    simd::float4 pitchQuat = axisAngleToQuat({1.0f, 0.0f, 0.0f}, pitchRad);
    
    // 再繞 Z 軸旋轉 (yaw)
    simd::float4 yawQuat = axisAngleToQuat({0.0f, 0.0f, 1.0f}, yawRad);
    
    rotation = quatMultiply(yawQuat, pitchQuat);
    updateVectors();
}

void Camera::walk(simd::float3 dPos)
{
    // 保持與原先 Euler 版本相同的 dPos 分量順序：
    // dPos[0] = forward/back, dPos[1] = right/left, dPos[2] = up/down
    simd::float3 movement = forwards * dPos[0] + right * dPos[1] + up * dPos[2];
    position += movement;
}

void Camera::spin(float dPitch, float dYaw)
{
    // 將增量轉為弧度
    float dPitchRad = dPitch * M_PI / 180.0f;
    float dYawRad = dYaw * M_PI / 180.0f;
    
    // 以本地 X 軸旋轉 (pitch)
    simd::float4 pitchQuat = axisAngleToQuat(right, dPitchRad);
    rotation = quatMultiply(pitchQuat, rotation);
    
    // 以世界 Z 軸旋轉 (yaw)
    simd::float4 yawQuat = axisAngleToQuat({0.0f, 0.0f, 1.0f}, dYawRad);
    rotation = quatMultiply(yawQuat, rotation);
    
    updateVectors();
}

simd::float4x4 Camera::getViewTransform()
{
    return mtlm::view_matrix(right, up, forwards, position);
}

simd::float4 Camera::axisAngleToQuat(simd::float3 axis, float angle)
{
    // 將軸角表示法轉換為四元數
    axis = simd::normalize(axis);
    float halfAngle = angle * 0.5f;
    float sinHalf = sin(halfAngle);
    
    return {
        axis[0] * sinHalf,
        axis[1] * sinHalf,
        axis[2] * sinHalf,
        cos(halfAngle)
    };
}

simd::float4 Camera::quatMultiply(simd::float4 q1, simd::float4 q2)
{
    // 四元數相乘
    // q1 = {x1, y1, z1, w1}, q2 = {x2, y2, z2, w2}
    return {
        q1[3] * q2[0] + q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1],
        q1[3] * q2[1] - q1[0] * q2[2] + q1[1] * q2[3] + q1[2] * q2[0],
        q1[3] * q2[2] + q1[0] * q2[1] - q1[1] * q2[0] + q1[2] * q2[3],
        q1[3] * q2[3] - q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2]
    };
}

simd::float3 Camera::rotateByQuat(simd::float3 v, simd::float4 q)
{
    // 使用四元數旋轉向量
    // v' = q * v * q^(-1)
    // 對於單位四元數，q^(-1) = {-x, -y, -z, w}
    
    simd::float4 vQuat = {v[0], v[1], v[2], 0.0f};
    simd::float4 qInv = {-q[0], -q[1], -q[2], q[3]};
    
    simd::float4 result = quatMultiply(quatMultiply(q, vQuat), qInv);
    
    return {result[0], result[1], result[2]};
}

void Camera::updateVectors()
{
    // 使用與原 Euler 實作對應的基向量來旋轉，確保 WASD 行為一致：
    // 前方基向量為 X 正方向，右方基向量為 Y 負方向
    forwards = rotateByQuat({1.0f, 0.0f, 0.0f}, rotation);
    right = rotateByQuat({0.0f, -1.0f, 0.0f}, rotation);
    up = rotateByQuat({0.0f, 0.0f, 1.0f}, rotation);
}
