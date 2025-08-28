#include "ClearScene.h"
using namespace KamataEngine;

void ClearScene::Initialize() {
	// カメラ初期化
	camera_.Initialize();

	// フェード初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// CLEARスプライト生成
	uint32_t clearHandle = TextureManager::Load("clear_text.png");
	clearSprite_ = Sprite::Create(clearHandle, {0.0f, 0.0f});                      // 適宜調整

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
}

void ClearScene::Draw() {

	// --- スプライト描画開始 ---
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());


	// CLEAR文字描画
	if (clearSprite_) {
		clearSprite_->Draw();
	}

	// --- スプライト描画終了 ---
	Sprite::PostDraw();

	// フェード描画
	if (fade_) {
		fade_->Draw();
	}
}

ClearScene::~ClearScene() {
	delete fade_;
	delete clearSprite_;
}
