#pragma once
#include "KamataEngine.h"
#include "math.h"

// 前方宣言
class Player;
class GameScene;
class MapChipField;

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

	// パトロール動作更新
	void BehaviorPatrolUpdate();

	// 追跡動作更新
	void BehaviorChaseUpdate();

	// 停止（ただし浮遊アニメーションあり）
	void BehaviorStopUpdate();

    // 飛行（上下移動）
    void BehaviorFlyUpdate();

	// 近づくと逃げる




	// デス演出更新
	void BehaviorDeathUpdate();

	// ステージ外に出ないよう位置を補正する
	void KeepWithinStage(MapChipField* mapChipField);

	// 歩行初期化
	void BehaviorRootInitialize();

	// デス演出初期化
	void BehaviorDeathInitialize();

	//ゲームシーンセッター
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

    // 追跡対象プレイヤー設定
    // 実装は Enemy.cpp に移動（Player は不完全型のためヘッダ内での使用を避ける）
    void SetTarget(Player* player);

	// 行動パターン設定
	enum class Pattern {
		kPatrol,
		kChase,
		kStop,
		kFly,
	};

	void SetPattern(Pattern p) { pattern_ = p; }

	// パトロール範囲設定
	void SetPatrolRange(float leftX, float rightX) { patrolLeftX_ = leftX; patrolRightX_ = rightX; }

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

	// 追跡時の速さ
	static inline const float kChaseSpeed = 0.035f;

    // 飛行（上下）振幅
    static inline const float kFlyAmplitude = 1.5f;

    // 飛行（上下）速度係数
    static inline const float kFlySpeed = 2.0f;

	// 逃走時の速さ




	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// 歩行演出最初の角度[度]
	static inline const float kWalkMotionAngleStart = 0.0f;

	// 歩行演出最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 30.0f;

	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 1.0f;

	// 浮遊（人魂っぽい）上下振幅
	static inline const float kFloatBobAmplitude = 0.3f;

	// 浮遊時の僅かな前後傾き（ラジアン、約5度）
	static inline const float kFloatTiltAmplitude = 0.0872664626f;

	// 経過時間
	float walkTimer_ = 0.0f;

	// 基準となるY座標（初期位置のY）
	float baseY_ = 0.0f;

    // （飛行）上下運動の基準高さ
    float baseHeight_ = 0.0f;

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

	// 追跡対象プレイヤー
	Player* target_ = nullptr;

	// 行動パターン
	Pattern pattern_ = Pattern::kPatrol;

	// パトロール用左右境界
	float patrolLeftX_ = 0.0f;
	float patrolRightX_ = 0.0f;

	// パトロール時の旋回アニメーション
	static inline const float kPatrolTurnTime = 0.3f;
	float patrolTurnTimer_ = 0.0f;
	float patrolStartRotationY_ = 0.0f;
	float patrolTargetRotationY_ = 0.0f;

	// 旋回後に適用する欲しい速度
	float patrolDesiredVelocityX_ = 0.0f;

	// モデルの基準向きオフセット(ラジアン)。モデルが正面向きなので左を向かせるには+pi/2 を指定
	// 使用するために定数リテラルで指定（std::numbers をヘッダで使うとコンパイル順依存になるため）
	static inline const float kModelFacingOffsetY = 1.57079632679f; // pi/2

};
