#define NOMINMAX
#include "Enemy.h"
#include "GameScene.h"
#include "Player.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#include <cmath>

using namespace KamataEngine;

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	camera_ = camera;

	// 速度を設定する
	velocity_ = {-kWalkSpeed, 0, 0};

    // 角度調整: 速度の向きに合わせる（移動する敵と同じ向きにする）
    worldTransform_.rotation_.y = (velocity_.x > 0.0f) ? (0.0f + kModelFacingOffsetY) : (std::numbers::pi_v<float> + kModelFacingOffsetY);

	walkTimer_ = 0.0f;
}

void Enemy::SetTarget(Player* player) {
    target_ = player;
    if (target_) {
        // プレイヤーが自分の左側にいる場合は左（pi）、右側にいる場合は右（0）を向く
        KamataEngine::Vector3 p = target_->GetWorldPosition();
        float dx = p.x - worldTransform_.translation_.x;
        worldTransform_.rotation_.y = (dx > 0.0f) ? (0.0f + kModelFacingOffsetY) : (std::numbers::pi_v<float> + kModelFacingOffsetY);
    }
}

void Enemy::Update() {

	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振るまいを変更する
		behavior_ = behaviorRequest_;
		// 各振るまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:

			BehaviorRootInitialize();

			break;

		case Behavior::kDeath:

			BehaviorDeathInitialize();

			break;
		}

		// 振るまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		// Choose pattern-based root behavior
		switch (pattern_) {
		case Pattern::kPatrol:
			BehaviorPatrolUpdate();
			break;
		case Pattern::kChase:
			BehaviorChaseUpdate();
			break;
		case Pattern::kShooter:
			BehaviorShooterUpdate();
			break;
		}

		break;

	case Behavior::kDeath:

		BehaviorDeathUpdate();

		break;
	}

	// アフィン変換とバッファ転送を一括でする関数
	math_.WorldTransformUpdate(worldTransform_);

	}

void Enemy::BehaviorPatrolUpdate() {
    // 単純左右往復
    worldTransform_.translation_ += velocity_;

    // 範囲反転
    if (worldTransform_.translation_.x < patrolLeftX_ || worldTransform_.translation_.x > patrolRightX_) {
        velocity_.x = -velocity_.x;
        // 旋回アニメーション開始
        patrolStartRotationY_ = worldTransform_.rotation_.y;
        patrolTargetRotationY_ = (velocity_.x > 0) ? 0.0f : std::numbers::pi_v<float>;
        patrolTurnTimer_ = kPatrolTurnTime;
        // 旋回終了後に適用したい速度を保存
        patrolDesiredVelocityX_ = velocity_.x;
    }

    // 歩行アニメーション
    walkTimer_ += 1.0f / 60.0f;
    worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime);

    // 旋回タイマーがあればイージングで回転
    if (patrolTurnTimer_ > 0.0f) {
        patrolTurnTimer_ -= 1.0f / 60.0f;
        float t = 1.0f - (patrolTurnTimer_ / kPatrolTurnTime);
        worldTransform_.rotation_.y = math_.easeInOut(t, patrolStartRotationY_, patrolTargetRotationY_);
    } else {
        // 旋回が終わった直後に希望する速度があれば適用
        if (std::abs(patrolDesiredVelocityX_) > 1e-6f) {
            velocity_.x = patrolDesiredVelocityX_;
            patrolDesiredVelocityX_ = 0.0f;
        }

        // 向き: 常に移動方向に合わせる（プレイヤー方向に合わせない）
        if (std::abs(velocity_.x) > 1e-6f) {
            worldTransform_.rotation_.y = (velocity_.x > 0) ? 0.0f + kModelFacingOffsetY : std::numbers::pi_v<float> + kModelFacingOffsetY;
        }
    }
}

void Enemy::BehaviorChaseUpdate() {
    if (!target_) {
        BehaviorPatrolUpdate();
        return;
    }

    // プレイヤーとの距離を計算
    KamataEngine::Vector3 playerPos = target_->GetWorldPosition();
    float dx = playerPos.x - worldTransform_.translation_.x;

    // 近ければ移動
    float dir = (dx > 0) ? 1.0f : -1.0f;
    velocity_.x = dir * kChaseSpeed;
    worldTransform_.translation_ += velocity_;

    // 向き: 移動方向に合わせる（プレイヤー方向に合わせない）
    if (std::abs(velocity_.x) > 1e-6f) {
        worldTransform_.rotation_.y = (velocity_.x > 0) ? 0.0f + kModelFacingOffsetY : std::numbers::pi_v<float> + kModelFacingOffsetY;
    }

    // 歩行アニメーション
    walkTimer_ += 1.0f / 60.0f;
    worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime);
}

void Enemy::BehaviorShooterUpdate() {
    if (!target_) {
        BehaviorPatrolUpdate();
        return;
    }

    // 距離を測って射程内ならタイマーでショット（現在はダミー）
    KamataEngine::Vector3 playerPos = target_->GetWorldPosition();
    float dx = playerPos.x - worldTransform_.translation_.x;
    float dz = playerPos.z - worldTransform_.translation_.z;
    float dist = std::sqrt(dx * dx + dz * dz);

    // 向き: 移動方向に合わせる（プレイヤー方向に合わせない）
    if (std::abs(velocity_.x) > 1e-6f) {
        worldTransform_.rotation_.y = (velocity_.x > 0) ? (0.0f + kModelFacingOffsetY) : (std::numbers::pi_v<float> + kModelFacingOffsetY);
    }

    // 射撃ロジック（タイマーのみ）
    // NOTE: 弾の生成処理は未実装のままにする（後で実装予定）
    deathTimer_ += 1.0f / 60.0f; // reuse timer as shoot timer
    if (dist <= kShootRange && deathTimer_ >= kShootInterval) {
        // TODO: 実際の弾生成処理をここに実装する
        // 現在は発射タイミングだけをリセットしておく
        deathTimer_ = 0.0f;
    }
}

    

void Enemy::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}

KamataEngine::Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	KamataEngine::Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Enemy::GetAABB() {
	KamataEngine::Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Enemy::OnCollision(const Player* player) {
	(void)player;

	if (behavior_ == Behavior::kDeath) {
		return;
	}

	// プレイヤーが攻撃中なら死ぬ
	if (player->IsAttack()) {
		// 敵の振る舞いをデス演出に変更
		behaviorRequest_ = Behavior::kDeath;

		// 敵と自キャラの中間位置にエフェクトを生成
		Vector3 effectPos = (GetWorldPosition() + player->GetWorldPosition()) / 2.0f;
		gameScene_->CreateHitEffect(effectPos);

		// コリジョン無効フラグを立てる
		isCollisionDisabled_ = true;
	}
}

void Enemy::BehaviorRootUpdate() {
	// 移動
	worldTransform_.translation_ += velocity_;

	// タイマーを加算
	walkTimer_ += 1.0f / 60.0f;

	// 回転アニメーション
	worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime);
}

void Enemy::BehaviorDeathUpdate() {

	// アニメーションのタイマーを加算する
	deathTimer_ += 1.0f / 60.0f;

	// Y軸周りの回転角をイージングで変化させる
	worldTransform_.rotation_.y = math_.easeInOut(deathTimer_, kDeathMotionAngleStart, kDeathMotionAngleEnd);

	// X軸周りの回転角をイージングで変化させる
	worldTransform_.rotation_.x = math_.easeInOut(deathTimer_, kDeathMotionAngleStart, kDeathMotionAngleEnd);

	// アニメーションのタイマーが一定時間に達したら
	if (deathTimer_ >= 1.0f) {
		isDead_ = true;
	}
}

void Enemy::BehaviorRootInitialize() {}

void Enemy::BehaviorDeathInitialize() {}
