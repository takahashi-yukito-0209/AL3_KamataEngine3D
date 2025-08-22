#pragma once
#include "Clone.h"
#include "KamataEngine.h"
#include "Player.h"

// --------------------------------------------
// ゲームシーンクラス
// プレイヤー操作、クローン表示、モード切替管理
// --------------------------------------------
class GameScene {
public:
	// 初期化
	void Initialize();

	// 更新（入力処理・切替処理）
	void Update();

	// 描画
	void Draw();

private:
	// プレイヤー
	Player player_;

	// クローン（1体固定）
	Clone clone_;

	// カメラ
	KamataEngine::Camera camera_;

	// リプレイモード切替
	enum class ReplayMode { kNone, kAuto, kFixed };
	ReplayMode replayMode_ = ReplayMode::kNone;

	// モード切替用の入力判定
	bool prevKeySpace_ = false;
};
