#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "math.h"

// クリアシーン
class ClearScene {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// 終了フラグ
	bool IsFinished() const { return finished_; }

private:
	// フェーズ管理
	enum class Phase {
		kFadeIn,  // フェードイン中
		kDisplay, // クリア表示中
		kFadeOut, // フェードアウト中
	};
	Phase phase_ = Phase::kFadeIn;

	// フェード用
	Fade* fade_ = nullptr;

	// 終了フラグ
	bool finished_ = false;

	// 表示時間（秒）
	static inline const float kDisplayTime = 2.0f;
	float timer_ = 0.0f;

	// カメラ
	KamataEngine::Camera camera_;

	// 文字モデル（"CLEAR" 表示用）
	KamataEngine::Model* modelText_ = nullptr;

	// 自作数学関数
	Math math_;
};
