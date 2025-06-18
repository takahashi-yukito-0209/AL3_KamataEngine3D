#pragma once
#include "KamataEngine.h"
#include "Math.h"
class TitleScene {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	//デストラクタ
	~TitleScene();

	//終了フラグのgetter
	bool IsFinished() const { return finished_; };

private:

	static inline const float kTimeTitleMove = 2.0f;

	// ビュープロジェクション
	KamataEngine::Camera camera_;
	KamataEngine::WorldTransform worldTransformTitle_;
	KamataEngine::WorldTransform worldTransformPlayer_;

	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelTitle_ = nullptr;

	float counter_ = 0.0f;
	
	//自作数学関数
	Math math_;

	//終了フラグ
	bool finished_ = false;
};
