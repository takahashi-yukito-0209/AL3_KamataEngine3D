#include "Player.h"
#include <cassert>

using namespace KamataEngine;

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle) {
	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

}

void Player::Update() {
	//キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	//キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	//押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	// 押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	//移動限界座標
	const float kMoveLimitX = 34.0f; // 左右移動可能範囲
	const float kMoveLimitY = 18.0f; // 上下移動可能範囲


	//範囲を超えない処理
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -kMoveLimitX, kMoveLimitX);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, -kMoveLimitY, kMoveLimitY);

	//座標移動(ベクトルの加算)
	worldTransform_.translation_ += move;

	// キャラクターの座標を画面表示する処理
	ImGui::Begin("Setting");
	ImGui::SliderFloat3("Player Translation", &worldTransform_.translation_.x, -100.0f,100.0f);
	ImGui::End();

	//アフィン変換行列の作成
	worldTransform_.matWorld_ = math_.MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_); 

	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw(KamataEngine::Camera* camera) { 
	model_->Draw(worldTransform_, *camera, textureHandle_);
}
