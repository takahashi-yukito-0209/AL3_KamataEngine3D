#include "Player.h"

using namespace KamataEngine;
// --------------------------------------------
// 初期化
// --------------------------------------------
void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	position_ = {0, 0, 0};
	worldTransform_.translation_ = position_;
}

// --------------------------------------------
// 更新（矢印キーによる移動）
// --------------------------------------------
void Player::Update() {
	velocity_ = {0, 0, 0};

	// 左右移動
	if (Input::GetInstance()->TriggerKey(DIK_LEFT)) {
		velocity_.x = -moveSpeed_;
	}
	if (Input::GetInstance()->TriggerKey(DIK_RIGHT)) {
		velocity_.x = moveSpeed_;
	}

	// 上下移動（2Dアクション用）
	if (Input::GetInstance()->TriggerKey(DIK_UP)) {
		velocity_.y = moveSpeed_;
	}
	if (Input::GetInstance()->TriggerKey(DIK_DOWN)) {
		velocity_.y = -moveSpeed_;
	}

	// 位置更新
	position_ = position_ + velocity_;
	worldTransform_.translation_ = position_;

	// 行列更新
	math_.WorldTransformUpdate(worldTransform_);
}

// --------------------------------------------
// 描画
// --------------------------------------------
void Player::Draw(KamataEngine::Camera* camera) {
	if (model_) {
		model_->Draw(worldTransform_, *camera);
	}
}
