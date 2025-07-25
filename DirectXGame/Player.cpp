#include "Player.h"
#include <cassert>

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle) {
	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;

	// ワールド変換の初期化
	worldTransform_.Initialize();

}

void Player::Update() {
	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw(KamataEngine::Camera* camera) { 
	model_->Draw(worldTransform_, *camera, textureHandle_);
}
