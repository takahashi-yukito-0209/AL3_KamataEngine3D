#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Math.h"

// ゲームシーン
class GameScene {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// デストラクタ
	~GameScene();

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;

	//自キャラ
	Player* player_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	// キーボード入力
	KamataEngine::Input* input_ = nullptr;

	//デバックカメラ有効
	bool isDebugCameraActive_ = false;

	//デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
};