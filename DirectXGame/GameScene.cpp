#include "GameScene.h"

using namespace KamataEngine;

// --------------------------------------------
// 初期化
// --------------------------------------------
void GameScene::Initialize() {
	// カメラ初期化
	camera_.Initialize();
	camera_.translation_ = {0, 0, -10.0f};
	camera_.UpdateMatrix();

	// プレイヤー初期化
	KamataEngine::Model playerModel;
	player_.Initialize(&playerModel, &camera_);

	// クローン初期化
	KamataEngine::Model cloneModel;
	clone_.Initialize(&cloneModel, &camera_);
}

// --------------------------------------------
// 更新（入力処理・モード切替）
// --------------------------------------------
void GameScene::Update() {
	// プレイヤー操作更新
	player_.Update();

	// モード切替（スペースキー）
	bool isPressSpace = Input::GetInstance()->TriggerKey(DIK_SPACE);
	if (isPressSpace && !prevKeySpace_) {
		// 切替
		if (replayMode_ == ReplayMode::kNone)
			replayMode_ = ReplayMode::kAuto;
		else if (replayMode_ == ReplayMode::kAuto)
			replayMode_ = ReplayMode::kFixed;
		else
			replayMode_ = ReplayMode::kNone;
	}
	prevKeySpace_ = isPressSpace;

	// クローン更新
	switch (replayMode_) {
	case ReplayMode::kNone:
		// 表示のみ固定
		break;
	case ReplayMode::kAuto:
		// プレイヤー位置追従（コピー再生型）
		//clone_.SetPosition(player_.GetPosition());
		break;
	case ReplayMode::kFixed:
		// クローン位置固定
		break;
	}
}

// --------------------------------------------
// 描画
// --------------------------------------------
void GameScene::Draw() {
	// プレイヤー描画
	player_.Draw(&camera_);

	// クローン描画
	clone_.Draw(&camera_);
}
