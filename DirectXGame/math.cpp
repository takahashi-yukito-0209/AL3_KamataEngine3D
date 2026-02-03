#include "math.h"
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace KamataEngine;

KamataEngine::Matrix4x4 Math::MakeTranslateMatrix(const KamataEngine::Vector3& translate) {
    Matrix4x4 r = {};
    r.m[0][0] = 1.0f; r.m[1][1] = 1.0f; r.m[2][2] = 1.0f; r.m[3][3] = 1.0f;
    r.m[3][0] = translate.x; r.m[3][1] = translate.y; r.m[3][2] = translate.z;
    return r;
}

KamataEngine::Matrix4x4 Math::MakeScaleMatrix(const KamataEngine::Vector3& scale) {
    Matrix4x4 r = {};
    r.m[0][0] = scale.x; r.m[1][1] = scale.y; r.m[2][2] = scale.z; r.m[3][3] = 1.0f;
    return r;
}

KamataEngine::Matrix4x4 Math::MakeRotateXMatrix(float radian) {
    Matrix4x4 r = {};
    r.m[0][0] = 1.0f;
    r.m[1][1] = std::cos(radian); r.m[1][2] = std::sin(radian);
    r.m[2][1] = std::sin(-radian); r.m[2][2] = std::cos(radian);
    r.m[3][3] = 1.0f;
    return r;
}

KamataEngine::Matrix4x4 Math::MakeRotateYMatrix(float radian) {
    Matrix4x4 r = {};
    r.m[0][0] = std::cos(radian); r.m[0][2] = std::sin(-radian);
    r.m[1][1] = 1.0f;
    r.m[2][0] = std::sin(radian); r.m[2][2] = std::cos(radian);
    r.m[3][3] = 1.0f;
    return r;
}

KamataEngine::Matrix4x4 Math::MakeRotateZMatrix(float radian) {
    Matrix4x4 r = {};
    r.m[0][0] = std::cos(radian); r.m[0][1] = std::sin(radian);
    r.m[1][0] = std::sin(-radian); r.m[1][1] = std::cos(radian);
    r.m[2][2] = 1.0f; r.m[3][3] = 1.0f;
    return r;
}

KamataEngine::Matrix4x4 Math::Multiply(const KamataEngine::Matrix4x4& a, const KamataEngine::Matrix4x4& b) {
    Matrix4x4 r = {};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            r.m[i][j] = 0.0f;
            for (int k = 0; k < 4; ++k) r.m[i][j] += a.m[i][k] * b.m[k][j];
        }
    }
    return r;
}

KamataEngine::Matrix4x4 Math::MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate) {
    Matrix4x4 rotateXYZ = Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));
    return Multiply(Multiply(MakeScaleMatrix(scale), rotateXYZ), MakeTranslateMatrix(translate));
}

KamataEngine::Vector3 Math::Transform(const KamataEngine::Vector3& v, const KamataEngine::Matrix4x4& m) {
    Vector3 r;
    r.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];
    r.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];
    r.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];
    float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];
    assert(w != 0.0f);
    r.x /= w; r.y /= w; r.z /= w;
    return r;
}

void Math::WorldTransformUpdate(KamataEngine::WorldTransform& worldTransform) {
    worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
    worldTransform.TransferMatrix();
}

float Math::easeInOut(float timer, float start, float end) {
    timer = std::clamp(timer, 0.0f, 1.0f);
    float easedT = timer * timer * (3.0f - 2.0f * timer);
    return (1.0f - easedT) * start + easedT * end;
}

KamataEngine::Vector3 Math::Lerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return {(1.0f - t) * v1.x + t * v2.x, (1.0f - t) * v1.y + t * v2.y, (1.0f - t) * v1.z + t * v2.z};
}

bool Math::IsCollision(const AABB& aabb1, const AABB& aabb2) {
    return (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
           (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
           (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z);
}

// Quaternion utilities (Math::Quaternion is declared in math.h)
Math::Quaternion Math::FromYaw(float yaw) {
    Math::Quaternion q;
    float half = yaw * 0.5f;
    q.x = 0.0f; q.y = std::sin(half); q.z = 0.0f; q.w = std::cos(half);
    return q;
}

Math::Quaternion Math::Slerp(const Math::Quaternion& q0, const Math::Quaternion& q1, float t) {
    float tt = std::clamp(t, 0.0f, 1.0f);
    float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
    Math::Quaternion b = q1;
    if (dot < 0.0f) { dot = -dot; b.x = -b.x; b.y = -b.y; b.z = -b.z; b.w = -b.w; }
    const float DOT_THRESHOLD = 0.9995f;
    Math::Quaternion result;
    if (dot > DOT_THRESHOLD) {
        result.x = q0.x + tt * (b.x - q0.x);
        result.y = q0.y + tt * (b.y - q0.y);
        result.z = q0.z + tt * (b.z - q0.z);
        result.w = q0.w + tt * (b.w - q0.w);
        float len = std::sqrt(result.x*result.x + result.y*result.y + result.z*result.z + result.w*result.w);
        result.x /= len; result.y /= len; result.z /= len; result.w /= len;
        return result;
    }
    float theta_0 = std::acos(dot);
    float theta = theta_0 * tt;
    float sin_theta = std::sin(theta);
    float sin_theta_0 = std::sin(theta_0);
    float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;
    result.x = (s0 * q0.x) + (s1 * b.x);
    result.y = (s0 * q0.y) + (s1 * b.y);
    result.z = (s0 * q0.z) + (s1 * b.z);
    result.w = (s0 * q0.w) + (s1 * b.w);
    return result;
}

float Math::ToYaw(const Math::Quaternion& q) {
    return std::atan2(2.0f * (q.w * q.y + q.z * q.x), 1.0f - 2.0f * (q.y * q.y + q.x * q.x));
}

// Vector operator overloads
KamataEngine::Vector3 operator+(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
KamataEngine::Vector3 operator-(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
KamataEngine::Vector3 operator*(const KamataEngine::Vector3& v, float s) { return {v.x * s, v.y * s, v.z * s}; }
KamataEngine::Vector3 operator/(const KamataEngine::Vector3& v, float s) { return {v.x / s, v.y / s, v.z / s}; }
KamataEngine::Vector3& operator+=(KamataEngine::Vector3& l, const KamataEngine::Vector3& r) { l.x += r.x; l.y += r.y; l.z += r.z; return l; }
KamataEngine::Vector3& operator-=(KamataEngine::Vector3& l, const KamataEngine::Vector3& r) { l.x -= r.x; l.y -= r.y; l.z -= r.z; return l; }
KamataEngine::Vector3& operator*=(KamataEngine::Vector3& v, float s) { v.x *= s; v.y *= s; v.z *= s; return v; }
KamataEngine::Vector3& operator/=(KamataEngine::Vector3& v, float s) { v.x /= s; v.y /= s; v.z /= s; return v; }
