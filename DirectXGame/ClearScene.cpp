#include "ClearScene.h"

using namespace KamataEngine;

void ClearScene::Initialize() {
	// カメラ初期化
	camera_.Initialize();

	// フェード初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// 文字モデル（"CLEAR"）生成
	modelText_ = Model::CreateFromOBJ("clear_text", true);

	// 初期フェーズ
	phase_ = Phase::kFadeIn;
	timer_ = 0.0f;
	finished_ = false;
}

void ClearScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			timer_ = 0.0f;
			phase_ = Phase::kDisplay;
		}
		break;

	case Phase::kDisplay:
		timer_ += 1.0f / 60.0f; // 1フレーム分進める
		if (timer_ >= kDisplayTime) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		break;

	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

	// カメラ行列更新
	camera_.UpdateMatrix();
}

void ClearScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// "CLEAR"文字描画
	if (modelText_) {
		KamataEngine::WorldTransform worldTransform;
		worldTransform.Initialize();
		worldTransform.translation_ = {0.0f, 5.0f, 0.0f}; // 適宜調整
		modelText_->Draw(worldTransform, camera_);
	}

	// フェード描画
	if (fade_) {
		fade_->Draw();
	}

	// 3Dモデル描画後処理
	Model::PostDraw();
}
