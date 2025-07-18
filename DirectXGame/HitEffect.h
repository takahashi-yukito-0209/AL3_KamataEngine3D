#pragma once
#include "KamataEngine.h"
#include "Math.h"

class HitEffect {
public:
	enum class State {
		kSpread,//拡大中
		kFade,//フェードアウト中
		kDead,//死亡
	};


	// 初期化
	void Initialize(const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	//デストラクタ
	~HitEffect();

	//静的メンバ関数
	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	//インスタンス生成と初期化
	static HitEffect* Create(const KamataEngine::Vector3& position);

	//デスフラグの取得
	bool IsDead() const { return state_ == State::kDead; }

private:

	//自作数学関数
	Math math_;

	// 拡大アニメーションの時間
	static inline const uint32_t kSpreadTime = 10;
	// フェードアウトアニメーションの時間
	static inline const uint32_t kFadeTime = 20;
	// エフェクトの寿命
	static inline const uint32_t kLifetime = kSpreadTime + kFadeTime;

	//モデル(借りてくる用)
	static KamataEngine::Model* model_;

	//カメラ(借りてくる用)
	static KamataEngine::Camera* camera_;

	//円のワールドトランスフォーム
	KamataEngine::WorldTransform circleWorldTransform_;

	// 楕円エフェクトの数
	static const inline uint32_t kellipseEffectNum = 2;

	//楕円のワールドトランスフォーム
	std::array<KamataEngine::WorldTransform, kellipseEffectNum> ellipseWorldTransforms_;

	//状態
	State state_ = State::kSpread;

	//カウンター
	uint32_t counter_ = 0;

	//色
	KamataEngine::ObjectColor objectColor_;
};
