#pragma once
#include "KamataEngine.h"
#include "math.h"

// 前方宣言
class Player;

class CameraController {
public:
	// 矩形
	struct Rect {
		float left = 0.0f;   // 左端
		float right = 1.0f;  // 右端
		float bottom = 0.0f; // 下端
		float top = 1.0f;    // 上端
	};

	// 初期化
	void Initialize(KamataEngine::Camera* camera);

	// 更新
	void Update();

	 // 追従対象
	void SetTarget(Player* target) { target_ = target; }
	
	// 瞬間合わせ
	void Reset();

	// カメラ移動範囲
	void SetMovableArea(Rect area) { movableArea_ = area; }

private:
	
	//カメラ
	KamataEngine::Camera* camera_ = nullptr;

	Player* target_ = nullptr;

	//数学関数
	Math math_;

	// 追従対象とカメラの座標の差（オフセット）
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};

	// カメラ移動範囲
	Rect movableArea_ = {0, 100, 0, 100};

	//  目標座標
	KamataEngine::Vector3 destination_;

    //  座標補間割合
	//  座標補間割合(未使用 - 旧式補間)
	static inline const float kInterpolationRate = 0.06f;

	//  速度掛け率
	// 追従先の速度影響を少し下げて自然にする
	static inline const float kVelocityBias = 18.0f;

	// カメラ用スプリング定数(振動数)
	static inline const float kSpringOmega = 12.0f;
	// 減衰係数(臨界減衰=1.0)
	static inline const float kSpringDamping = 1.0f;

	// カメラ速度(スプリングでの速度)
	KamataEngine::Vector3 cameraVelocity_ = {0.0f, 0.0f, 0.0f};

	//  追従対象の各方向へのカメラ移動範囲
	static inline const Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};
};
