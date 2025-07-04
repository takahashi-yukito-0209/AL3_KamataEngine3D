#pragma once
#include "KamataEngine.h"
#include "Math.h"

class HitEffect {
public:
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

private:

	//自作数学関数
	Math math_;

	//モデル(借りてくる用)
	static KamataEngine::Model* model_;

	//カメラ(借りてくる用)
	static KamataEngine::Camera* camera_;

	//円のワールドトランスフォーム
	KamataEngine::WorldTransform circleWorldTransform_;
};
