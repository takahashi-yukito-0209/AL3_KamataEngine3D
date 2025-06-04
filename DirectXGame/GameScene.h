#pragma once
#include "CameraController.h"
#include "KamataEngine.h"
#include "MapChipFiled.h"
#include "Player.h"
#include "Skydome.h"
#include "math.h"
#include <vector>

// ゲームシーン
class GameScene {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// 表示ブロックの生成
	void GenerateBlocks();

	// デストラクタ
	~GameScene();

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// ブロック3Dモデル
	KamataEngine::Model* modelBlock_ = nullptr;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// カメラ
	KamataEngine::Camera camera_;

	// 自キャラ
	Player* player_ = nullptr;

	// 自キャラ3Dモデル
	KamataEngine::Model* modelPlayer_ = nullptr;

	// 配列の生成
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	// 自作した数学関数
	Math math_;

	// デバックカメラ有効
	bool isDebugCameraActive_ = false;

	// デバックカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// スカイドーム
	Skydome* skydome_ = nullptr;

	// スカイドーム3Dモデル
	KamataEngine::Model* modelSkydome_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	//カメラコントローラー
	CameraController* cameraController_;
};