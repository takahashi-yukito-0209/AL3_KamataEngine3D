#include "HitEffect.h"
#include <numbers>
#include <random>

using namespace KamataEngine;

// 静的メンバ変数の実態
KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;

void HitEffect::Initialize(const KamataEngine::Vector3& position) {

	// 乱数生成エンジン
	std::random_device seedGenerator;
	// メルセンヌ・ツイスターエンジン(64bit版)
	std::mt19937_64 randomEngine;
	// メルセンヌ・ツイスターエンジンの初期化
	randomEngine.seed(seedGenerator());
	// 指定範囲の乱数生成器(浮動小数点数用)
	std::uniform_real_distribution<float> rotationDistribution(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	// 楕円エフェクト
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		// 乱数の取得と初期化
		worldTransform.Initialize();
		worldTransform.rotation_ = {0.0f, 3.14159265f, rotationDistribution(randomEngine)};
		worldTransform.translation_ = position;
		worldTransform.translation_.z += 0.5f;
	}

	// 円形エフェクト
	// 円形エフェクトのワールドトランスフォームを発生座標で初期化
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = position;
	circleWorldTransform_.rotation_.y = 3.14159265f; // なぜか裏向きで描画されるため反転させる

	// 色の初期化
	objectColor_.Initialize();
}

void HitEffect::Update() {

	if (IsDead()) {
		return; // すでに消滅している場合は更新しない
	}

	// 大きさの設定
	float scale = 0.5f + static_cast<float>(counter_) / kSpreadTime * 0.5f;
	const float slashScale = 2.0f;
	const float circleScale = 1.0f;

	switch (state_) {
	case State::kSpread:
		// カウンター加算
		++counter_;

		// スケールの更新
		for (auto& slashWorldTransform : ellipseWorldTransforms_) {
			slashWorldTransform.scale_ = {0.1f, scale * slashScale, 1.0f};
		}

		circleWorldTransform_.scale_ = {scale * circleScale, scale * circleScale, 1.0f};

		// カウンターが拡大アニメーションの時間を越えたら
		if (counter_ >= kSpreadTime) {
			state_ = State::kFade;
			counter_ = 0; // カウンターをリセット
		}

		break;

	case State::kFade:
		// カウンターの加算
		++counter_;

		// 徐々にフェードさせる
		objectColor_.SetColor(Vector4{1.0f, 1.0f, 1.0f, 1.0f - static_cast<float>(counter_) / kFadeTime});

		// カウンターがフェードアウトアニメーションの時間を越えたら
		if (++counter_ >= kFadeTime) {
			state_ = State::kDead;
		}

		break;
	}

	// アフィン変換とバッファ転送を一括でする関数
	for (auto& ellipseWorldTransform : ellipseWorldTransforms_) {
		math_.WorldTransformUpdate(ellipseWorldTransform);
	}
	math_.WorldTransformUpdate(circleWorldTransform_);
}

void HitEffect::Draw() {

	if (IsDead()) {
		return; // すでに消滅している場合は描画しない
	}

	for (auto& ellipseWorldTransform : ellipseWorldTransforms_) {
		model_->Draw(ellipseWorldTransform, *camera_, &objectColor_);
	}

	// 3Dモデルを描画
	model_->Draw(circleWorldTransform_, *camera_, &objectColor_);
}

HitEffect::~HitEffect() {}

HitEffect* HitEffect::Create(const KamataEngine::Vector3& position) {
	// インスタンス生成
	HitEffect* instance = new HitEffect();
	// newの失敗を検出
	assert(instance);
	// インスタンスの初期化
	instance->Initialize(position);
	// 初期化したインスタンスを返す
	return instance;
}
