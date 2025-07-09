#pragma once
#include "KamataEngine.h"
#include "math.h"

// 前方宣言
class MapChipField;
class Enemy;

class Player {
public:
	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上
		kNumCorner    // 要素数
	};

	// 振るまい
	enum class Behavior {
		kUnknown = -1,
		kRoot,   // 通常状態
		kAttack, // 攻撃中
	};

	// 攻撃フェーズ
	enum class AttackPhase {
		kUnknown = -1, // 無効な状態
		kAnticipation, // 予備動作
		kAction,       // 前進動作
		kRecovery,     // 余韻動作
	};

	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelAttack, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	// ---getter---

	// WorldTransform
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; };

	// Velocity
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; };

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition() const;

	// AABB取得関数
	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Enemy* enemy);

	// デスフラグ
	bool IsDead() const { return isDead_; }

	//攻撃中かの判定
	bool IsAttack() const { return behavior_ == Behavior::kAttack; }

	// ---setter---

	// MapChipField
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// 通常行動更新
	void BehaviorRootUpdate();

	// 攻撃行動更新
	void BehaviorAttackUpdate();

	// 通常行動初期化
	void BehaviorRootInitialize();

	// 攻撃行動初期化
	void BehaviorAttackInitialize();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 自作した数学関数
	Math math_;

	// 移動量
	KamataEngine::Vector3 velocity_ = {};

	// 加速度
	static inline const float kAcceleration = 0.1f;

	// 非入力時の摩擦係数
	static inline const float kAttenuation = 0.05f;

	// 最大速度
	static inline const float kLimitRunSpeed = 0.3f;

	// 初期の向いている方向
	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の速度
	float turnFirstRotationY_ = 0.0f;

	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	// 接地状態フラグ
	bool onGround_ = true;

	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 9.8f;

	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 0.5f;

	// ジャンプ初速(下方向)
	static inline const float kJumpAcceleration = 1.0f;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 空白
	static inline const float kBlank = 0.04f;

	// 移動入力関数
	void InputMove();

	// マップとの当たり判定情報
	struct CollisionMapInfo {

		bool ceiling = false; // 天井衝突フラグ
		bool landing = false; // 着地フラグ
		bool hitWall = false; // 壁接触フラグ

		KamataEngine::Vector3 move; // 移動量
	};

	// マップ衝突判定関数
	void CheckMapCollision(CollisionMapInfo& info);

	// マップ衝突判定上方向
	void CheckMapCollisionUp(CollisionMapInfo& info);

	// マップ衝突判定下方向
	void CheckMapCollisionDown(CollisionMapInfo& info);

	// マップ衝突判定右方向
	void CheckMapCollisionRight(CollisionMapInfo& info);

	// マップ衝突判定左方向
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	// 指定した角の座標計算関数
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	// 接地状態の切り替え処理関数
	void UpdateOnGround(const CollisionMapInfo& info);

	// 壁接触している場合の処理
	void UpdateOnWall(const CollisionMapInfo& info);

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.0f;

	// 微小な数値
	static inline const float kGroundSearchHeight = 0.06f;

	// 着地時の速度減衰率
	static inline const float kAttenuationWall = 0.2f;

	// デスフラグ
	bool isDead_ = false;

	// 振るまい
	Behavior behavior_ = Behavior::kRoot;

	// 次の振るまいリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// 攻撃ギミックの経過時間カウンター
	uint32_t attackParameter_ = 0;

	// 攻撃フェーズ
	AttackPhase attackPhase_ = AttackPhase::kUnknown;

	//予備動作の時間
	static inline const uint32_t kAnticipationTime = 8;

	// 前進動作の時間
	static inline const uint32_t kActionTime = 5;
	
	// 余韻動作の時間
	static inline const uint32_t kRecoveryTime = 12;

	// 攻撃用ワールド変換データ
	KamataEngine::WorldTransform worldTransformAttack_;

	// プレイヤー攻撃用モデル
	KamataEngine::Model* modelAttack_ = nullptr;

};
