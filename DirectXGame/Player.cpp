#include "Player.h"
#include <cassert>

using namespace KamataEngine;

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera) {
	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	camera_ = camera;
}

void Player::Update() {
	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() {

	//3Dモデルを描画
	model_->Draw(worldTransform_, *camera_,textureHandle_);

}
