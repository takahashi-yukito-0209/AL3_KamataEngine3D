#pragma once
#include "KamataEngine.h"
#include "math.h"

class Enemy {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	//歩行の速さ
	static inline const float kWalkSpeed = 0.01f;

	//速度
	KamataEngine::Vector3 velocity_ = {};

	//最初の角度[度]
	static inline const float kWalkMotionAngleStart = 0.0f;

	//最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 30.0f;

	//アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 1.0f;

	//経過時間
	float walkTimer_ = 0.0f;

	// 自作した数学関数
	Math math_;
};
