#pragma once
#include "KamataEngine.h"
#include "math.h"

class Player {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera,const KamataEngine::Vector3& position);

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

	// 自作した数学関数
	Math math_;

	//移動量
	KamataEngine::Vector3 velocity_ = {};

	//加速度
	static inline const float kAcceleration = 0.08f;

	//速度減衰率
	static inline const float kAttenuation = 0.06f;

	//最大速度制限
	static inline const float kLimitRunSpeed = 0.1f;

	//左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	//初期の向いている方向
	LRDirection lrDirection_ = LRDirection::kRight;

	//旋回開始時の速度
	float turnFirstRotationY_ = 0.0f;

	//旋回タイマー
	float turnTimer_ = 0.0f;

	//旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	//接地状態フラグ
	bool onGround_ = true;

	//重力加速度(下方向)
	static inline const float kGravityAcceleration = 9.8f;

	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 0.1f;

	// ジャンプ初速(下方向)
	static inline const float kJumpAcceleration = 2.0f;
};
