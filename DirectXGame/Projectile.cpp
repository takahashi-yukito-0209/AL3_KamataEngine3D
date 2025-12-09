#define NOMINMAX
#include "Projectile.h"
#include <cassert>
#include <algorithm>
#include <numbers>

using namespace KamataEngine;

void Projectile::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
    assert(model);
    model_ = model;
    camera_ = camera;
    worldTransform_.Initialize();
    worldTransform_.translation_ = position;
    // モデル向きが裏向きになっている場合があるため、Y回転を180度回す
    worldTransform_.rotation_.y = std::numbers::pi_v<float>;
    // 目立つように少し大きめのスケールにする
    worldTransform_.scale_ = {0.6f, 0.6f, 0.6f};
    // 色を黒に設定
    objectColor_.Initialize();
    objectColor_.SetColor({0.0f, 0.0f, 0.0f, 1.0f});
}

void Projectile::Update() {
    if (isDead_) return;

    // 移動
    worldTransform_.translation_ += velocity_;

    // 生存時間減少
    lifeTime_ -= 1.0f / 60.0f;
    if (lifeTime_ <= 0.0f) {
        isDead_ = true;
    }

    // 行列更新
    math_.WorldTransformUpdate(worldTransform_);
}

void Projectile::Draw() {
    if (isDead_) return;
    model_->Draw(worldTransform_, *camera_, &objectColor_);
}

AABB Projectile::GetAABB() const {
    AABB aabb;
    aabb.min = {worldTransform_.translation_.x - kWidth/2.0f, worldTransform_.translation_.y - kHeight/2.0f, worldTransform_.translation_.z - kWidth/2.0f};
    aabb.max = {worldTransform_.translation_.x + kWidth/2.0f, worldTransform_.translation_.y + kHeight/2.0f, worldTransform_.translation_.z + kWidth/2.0f};
    return aabb;
}
