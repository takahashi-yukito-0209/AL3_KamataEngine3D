#pragma once
#include "KamataEngine.h"
#include "math.h"

// 前方宣言
class Player;
class GameScene;

class Enemy {
public:
	// 振るまい
	enum class Behavior {
		kUnknown = -1,
		kRoot,  // 歩行
		kDeath, // デス演出
	};

	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition();

	// デスフラグ
	bool IsDead() const { return isDead_; }

	//無効フラグ
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

	// AABB取得関数
	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Player* player);

	// 歩行更新
	void BehaviorRootUpdate();

	// デス演出更新
	void BehaviorDeathUpdate();

	// 歩行初期化
	void BehaviorRootInitialize();

	// デス演出初期化
	void BehaviorDeathInitialize();

	//ゲームシーンセッター
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.01f;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// 歩行演出最初の角度[度]
	static inline const float kWalkMotionAngleStart = 0.0f;

	// 歩行演出最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 30.0f;

	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 1.0f;

	// 経過時間
	float walkTimer_ = 0.0f;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 自作した数学関数
	Math math_;

	// デスフラグ
	bool isDead_ = false;

	// 振るまい
	Behavior behavior_ = Behavior::kRoot;

	// 次の振るまいリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// デス演出タイマー
	float deathTimer_ = 0.0f;

	// デス演出最初の角度[度]
	static inline const float kDeathMotionAngleStart = 0.0f;

	// デス演出最後の角度[度]
	static inline const float kDeathMotionAngleEnd = 30.0f;

	//無効フラグ
	bool isCollisionDisabled_ = false;

	GameScene* gameScene_ = nullptr;

};
