#include "HitEffect.h"

using namespace KamataEngine;

// 静的メンバ変数の実態
KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;

void HitEffect::Initialize(const KamataEngine::Vector3& position) {

	//円形エフェクト
	//円形エフェクトのワールドトランスフォームを発生座標で初期化
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = position;

}

void HitEffect::Update() {

	// アフィン変換とバッファ転送を一括でする関数
	math_.WorldTransformUpdate(circleWorldTransform_);
}

void HitEffect::Draw() {
	// 3Dモデルを描画
	model_->Draw(circleWorldTransform_, *camera_);
}

HitEffect::~HitEffect() {
	
}

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
