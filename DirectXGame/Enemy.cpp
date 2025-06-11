#define NOMINMAX
#include "Enemy.h"
#include <algorithm>
#include <cassert>
#include <numbers>

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	camera_ = camera;

	//速度を設定する
	velocity_ = {-kWalkSpeed, 0, 0};

	// 角度調整
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	walkTimer_ = 0.0f;
}

void Enemy::Update() {

	//移動
	worldTransform_.translation_ += velocity_;

	//タイマーを加算
	walkTimer_ += 1.0f / 60.0f;

	//回転アニメーション
	worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime);

	// アフィン変換とバッファ転送を一括でする関数
	math_.WorldTransformUpdate(worldTransform_);
}

void Enemy::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}
