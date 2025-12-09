#pragma once
#include "KamataEngine.h"
#include "math.h"

class Projectile {
public:
    void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
    void Update();
    void Draw();
    AABB GetAABB() const;
    bool IsDead() const { return isDead_; }
    void SetVelocity(const KamataEngine::Vector3& v) { velocity_ = v; }

private:
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* model_ = nullptr;
    KamataEngine::Camera* camera_ = nullptr;
    Math math_;
    KamataEngine::Vector3 velocity_ = {};
    float lifeTime_ = 3.0f; // seconds
    bool isDead_ = false;

    static inline const float kWidth = 0.2f;
    static inline const float kHeight = 0.2f;
    // 色変更オブジェクト
    KamataEngine::ObjectColor objectColor_;
};
