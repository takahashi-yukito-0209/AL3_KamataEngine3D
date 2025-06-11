#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include <algorithm>
#include <array>
#include <numbers>

class DeathParticles {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	// パーティクル座標配列
	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 存続時間（消滅までの時間）<秒>
	static inline const float kDuration_ = 2.0f;

	// 移動の速さ
	static inline const float kSpeed_ = 0.05f;

	// 分割した1個分の角度
	static inline const float kAngleUnit_ = 2.0f * std::numbers::pi_v<float> / kNumParticles;

	// 終了フラグ
	 bool isFinished_ = false;

	// 経過時間カウント
	 float counter_ = 0.0f;

	// 色変更オブジェクト
	 KamataEngine::ObjectColor objectColor_;

	// 色の数値
	 KamataEngine::Vector4 color_;

	// 自作数学関数
	Math math_;
};
