#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Math.h"

// --------------------------------------------
// プレイヤークラス
// --------------------------------------------
class Player {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);

	// 更新（入力処理）
	void Update();

	// 描画
	void Draw(KamataEngine::Camera* camera);

	// 現在座標取得
	const KamataEngine::Vector3& GetPosition() const { return position_; }

private:
	// ワールド変換
	KamataEngine::WorldTransform worldTransform_;

	// 描画用モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ参照
	KamataEngine::Camera* camera_ = nullptr;

	// 現在の座標
	KamataEngine::Vector3 position_ = {0, 0, 0};

	// 移動速度
	KamataEngine::Vector3 velocity_ = {0, 0, 0};

	// 移動量
	float moveSpeed_ = 0.1f;

	Math math_;
};
