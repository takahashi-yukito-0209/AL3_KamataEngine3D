#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "math.h"

// --------------------------------------------
// カメラコントローラクラス
// プレイヤーに追従させる処理を担当
// --------------------------------------------
class CameraController {
public:
	// 追従可能範囲の矩形
	struct Rect {
		float left = 0.0f;
		float right = 100.0f;
		float bottom = 0.0f;
		float top = 100.0f;
	};

	// 初期化
	void Initialize(KamataEngine::Camera* camera);

	// 毎フレーム更新
	void Update();

	// プレイヤーに瞬間的に合わせる
	void Reset();

	// 追従対象設定
	void SetTarget(Player* target) { target_ = target; }

	// カメラ移動可能範囲設定
	void SetMovableArea(Rect area) { movableArea_ = area; }

private:
	KamataEngine::Camera* camera_ = nullptr; // カメラ
	Player* target_ = nullptr;               // 追従対象
	Math math_;                              // 自作数学関数

	// カメラオフセット（プレイヤー後方）
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};

	// 移動可能範囲
	Rect movableArea_ = {0, 100, 0, 100};

	// 現在の目的地座標
	KamataEngine::Vector3 destination_;

	// 補間速度
	static inline const float kInterpolationRate = 0.1f;

	// 速度補正係数
	static inline const float kVelocityBias = 30.0f;

	// プレイヤー追従マージン
	static inline const Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};
};
