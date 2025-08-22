#pragma once
#include "KamataEngine.h"
#include <vector>
#include "Math.h"

// --------------------------------------------
// クローンクラス
// プレイヤーの動きを履歴として記録し、
// 再生 / 巻き戻し / 静止 のモードを持つ
// --------------------------------------------
class Clone {
public:
	// 挙動モード
	enum class Mode {
		kNone,     // 静止
		kPlayback, // 再生
		kRewind    // 巻き戻し
	};

	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);

	// 更新（プレイヤー座標を受け取り、モードに応じて挙動を変える）
	void Update(const KamataEngine::Vector3& playerPos);

	// 描画
	void Draw(KamataEngine::Camera* camera);

	// モード切替
	void SetMode(Mode mode) { mode_ = mode; }

private:
	// ワールド変換
	KamataEngine::WorldTransform worldTransform_;

	// 描画用モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ参照
	KamataEngine::Camera* camera_ = nullptr;

	// 過去のプレイヤー位置履歴
	std::vector<KamataEngine::Vector3> history_;

	// 再生 / 巻き戻しのインデックス
	size_t currentIndex_ = 0;

	// 現在のモード
	Mode mode_ = Mode::kNone;

	Math math_;
};
