#define NOMINMAX
#include "Enemy.h"
#include "GameScene.h"
#include "Player.h"
#include "MapChipFiled.h"
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

	// 基準Yを記録して上下の浮遊に使う
	baseY_ = position.y;
	// 基準高さを記録（飛行の上下運動用）
	baseHeight_ = position.y;
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


void Enemy::BehaviorStopUpdate() {
    // 停止だけど浮遊アニメーションは続ける
    // 速度を0にして浮遊のみ行う
    velocity_.x = 0.0f;

    walkTimer_ += 1.0f / 60.0f;
    worldTransform_.translation_.y = baseY_ + std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime) * kFloatBobAmplitude;
    worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime) * kFloatTiltAmplitude;
}

void Enemy::BehaviorFlyUpdate() {
    // 上下移動（浮遊とは別に大きく上下する飛行）
    walkTimer_ += 1.0f / 60.0f;
    float yOffset = std::sin(walkTimer_ * kFlySpeed) * kFlyAmplitude;
    worldTransform_.translation_.y = baseHeight_ + yOffset;

    // 傾きの表現は既存の浮遊と合わせる
    worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime) * kFloatTiltAmplitude;
}
// BehaviorFleeUpdate は削除済み

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
		// パターンに基づく root 振る舞いを選択
		switch (pattern_) {
		case Pattern::kPatrol:
			BehaviorPatrolUpdate();
			break;
		case Pattern::kChase:
			BehaviorChaseUpdate();
			break;
	case Pattern::kStop:
		BehaviorStopUpdate();
		break;
	case Pattern::kFly:
		BehaviorFlyUpdate();
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

    // 浮遊アニメーション（上下に bob ）
    walkTimer_ += 1.0f / 60.0f;
    worldTransform_.translation_.y = baseY_ + std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime) * kFloatBobAmplitude;
    // 少しだけ前後に傾ける（人魂の揺れ感）
    worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime) * kFloatTiltAmplitude;

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
    // 水平方向はプレイヤーへ向かう向きで移動
    float dir = (dx > 0) ? 1.0f : -1.0f;
    velocity_.x = dir * kChaseSpeed;
    worldTransform_.translation_.x += velocity_.x;

    // 垂直方向（Y）は徐々にプレイヤーの高さに近づける
    // lerp でスムーズに中心高さをプレイヤーに近づける
    baseY_ = std::lerp(baseY_, playerPos.y, 0.02f);

    // 向き: 移動方向に合わせる
    if (std::abs(velocity_.x) > 1e-6f) {
        worldTransform_.rotation_.y = (velocity_.x > 0) ? 0.0f + kModelFacingOffsetY : std::numbers::pi_v<float> + kModelFacingOffsetY;
    }

    // 浮遊アニメーション（上下に bob ）を baseY_ を中心に適用
    walkTimer_ += 1.0f / 60.0f;
    worldTransform_.translation_.y = baseY_ + std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime) * kFloatBobAmplitude;
    // 少しだけ前後に傾ける（人魂の揺れ感）
    worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime) * kFloatTiltAmplitude;
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

    // タイマーを加算して上下に浮遊させる
    walkTimer_ += 1.0f / 60.0f;
    worldTransform_.translation_.y = baseY_ + std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime) * kFloatBobAmplitude;
    // 少しだけ前後に傾ける（人魂の揺れ感）
    worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime) * kFloatTiltAmplitude;
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

void Enemy::KeepWithinStage(MapChipField* mapChipField) {
    if (!mapChipField) return;

    // Clamp X within [0, width)
    const uint32_t w = mapChipField->GetNumBlockHorizontal();
    const uint32_t h = mapChipField->GetNumBlockVirtical();

    // Compute min/max world coordinates from map bounds
    KamataEngine::Vector3 leftBottom = mapChipField->GetMapChipPositionByIndex(0, h - 1);
    KamataEngine::Vector3 rightTop = mapChipField->GetMapChipPositionByIndex(w - 1, 0);

    float minX = leftBottom.x - 0.5f; // small padding
    float maxX = rightTop.x + 0.5f;
    float minY = rightTop.y - 0.5f;
    float maxY = leftBottom.y + 0.5f;

    // Clamp translation
    worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, minX, maxX);
    worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, minY, maxY);

    // 前方にブロックがあれば反転させる（徘徊する敵がブロックで折り返す挙動）
    // 現在のブロックインデックスを取得
    IndexSet idx = mapChipField->GetMapChipIndexSetByPosition(worldTransform_.translation_);
    // 左右のチェック
    if (velocity_.x > 0.0f) {
        // 右側のマップチップをチェック
        if (idx.xIndex + 1 < w) {
            if (mapChipField->GetMapChipTypeByIndex(idx.xIndex + 1, idx.yIndex) == MapChipType::kBlock) {
                velocity_.x = -velocity_.x;
                // 向きを即座に反転
                worldTransform_.rotation_.y = std::numbers::pi_v<float> + kModelFacingOffsetY;
            }
        }
    } else if (velocity_.x < 0.0f) {
        if (idx.xIndex > 0) {
            if (mapChipField->GetMapChipTypeByIndex(idx.xIndex - 1, idx.yIndex) == MapChipType::kBlock) {
                velocity_.x = -velocity_.x;
                worldTransform_.rotation_.y = 0.0f + kModelFacingOffsetY;
            }
        }
    }
}
