#pragma once
#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "KamataEngine.h"
#include "MapChipFiled.h"
#include "Player.h"
#include "Skydome.h"
#include "math.h"
#include <vector>
#include "Fade.h"

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

	// 総当たりによる当たり判定
	void CheckAllCollisions();

	// 終了フラグのgetter
	bool IsFinished() const { return finished_; };

private:

	// ゲームのフェーズ
	enum class Phase {
		kFadeIn,//フェードイン
		kPlay,  // ゲームプレイ
		kDeath, // デス演出
		kFadeOut,//フェードアウト
	};

	//ゲームの現在フェーズ
	Phase phase_;

	//フェーズの切り替え関数
	void ChangePhase();

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

	// カメラコントローラー
	CameraController* cameraController_;

	// 敵
	std::list<Enemy*> enemies_;

	// 敵キャラ3Dモデル
	KamataEngine::Model* modelEnemy_ = nullptr;

	// デスパーティクル
	DeathParticles* deathParticles_ = nullptr;

	// デスパーティクル3Dモデル
	KamataEngine::Model* modelDeathParticles_ = nullptr;

	// 終了フラグ
	bool finished_ = false;

	//フェード
	Fade* fade_ = nullptr;
};