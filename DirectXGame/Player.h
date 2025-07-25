#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include "input/Input.h"

class Player {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model,uint32_t textureHandle);

	// 更新
	void Update();

	// 描画
	void Draw(KamataEngine::Camera* camera_);

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	//モデル
	KamataEngine::Model* model_ = nullptr;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	Math math_;

	//キーボード入力
	KamataEngine::Input* input_ = nullptr;

};
