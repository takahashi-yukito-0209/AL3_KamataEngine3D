#define NOMINMAX
#include "Enemy.h"
#include "GameScene.h"
#include "Player.h"
#include <algorithm>
#include <cassert>
#include <numbers>

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

	// 角度調整
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	walkTimer_ = 0.0f;
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
		BehaviorRootUpdate();

		break;

	case Behavior::kDeath:

		BehaviorDeathUpdate();

		break;
	}

	// アフィン変換とバッファ転送を一括でする関数
	math_.WorldTransformUpdate(worldTransform_);
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
