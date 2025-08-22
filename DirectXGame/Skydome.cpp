#include "Skydome.h"
#include <cassert>

using namespace KamataEngine;

void Skydome::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	camera_ = camera;
}

void Skydome::Update() {

	// アフィン変換行列の作成
	worldTransform_.matWorld_ = math_.MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Skydome::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}
