#include "HitEffect.h"
#include <random>
#include <numbers>

using namespace KamataEngine;

// 静的メンバ変数の実態
KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;

void HitEffect::Initialize(const KamataEngine::Vector3& position) {

	std::random_device seedGenerator;
	std::mt19937_64 randomEngine;
	randomEngine.seed(seedGenerator());
	std::uniform_real_distribution<float> rotationDistribution(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	//楕円エフェクト
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.rotation_ = {0.0f, 3.14159265f, rotationDistribution(randomEngine)};
		worldTransform.translation_ = position;
		worldTransform.Initialize();
	
	}

	// 円形エフェクト
	// 円形エフェクトのワールドトランスフォームを発生座標で初期化
	circleWorldTransform_.translation_ = position;
	circleWorldTransform_.rotation_.y = 3.14159265f; // なぜか裏向きで描画されるため反転させる
	circleWorldTransform_.Initialize();

}

void HitEffect::Update() {

	// アフィン変換とバッファ転送を一括でする関数
	for (auto& ellipseWorldTransform : ellipseWorldTransforms_) {
		math_.WorldTransformUpdate(ellipseWorldTransform);
	}
	math_.WorldTransformUpdate(circleWorldTransform_);
}

void HitEffect::Draw() {

	for (auto& ellipseWorldTransform : ellipseWorldTransforms_) {
		model_->Draw(ellipseWorldTransform, *camera_);
	}

	// 3Dモデルを描画
	model_->Draw(circleWorldTransform_, *camera_);
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
