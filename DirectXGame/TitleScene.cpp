#include "TitleScene.h"
#include <numbers>

using namespace KamataEngine;

void TitleScene::Initialize() {
	modelTitle_ = Model::CreateFromOBJ("titleFont", true);
	modelPlayer_ = Model::CreateFromOBJ("player");

	// カメラ初期化
	camera_.Initialize();

	const float kPlayerTitle = 2.0f;

	worldTransformTitle_.Initialize();

	worldTransformTitle_.scale_ = {kPlayerTitle, kPlayerTitle, kPlayerTitle};

	const float kPlayerScale = 10.0f;

	worldTransformPlayer_.Initialize();

	worldTransformPlayer_.scale_ = {kPlayerScale, kPlayerScale, kPlayerScale};

	worldTransformPlayer_.rotation_.y = 0.95f * std::numbers::pi_v<float>;

	worldTransformPlayer_.translation_.x = -2.0f;

	worldTransformPlayer_.translation_.y = -10.0f;

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// 操作ガイドスプライト生成（右下に表示）
	uint32_t guideHandle = TextureManager::Load("operation_guide.png");
	operationGuide_ = Sprite::Create(guideHandle, {1000.0f, 460.0f});
	operationGuide_->SetSize({250.0f, 250.0f});
}

void TitleScene::Update() {

	// フェードの更新
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();

		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}

		break;

	case Phase::kMain:

		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}

		break;

	case Phase::kFadeOut:
		fade_->Update();
		// タイトルシーンの終了条件
		// スペースキーを押したらシーンを終わる
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		break;
	}

	counter_ += 1.0f / 60.0f;
	counter_ = std::fmod(counter_, kTimeTitleMove);

	float angle = counter_ / kTimeTitleMove * 2.0f * std::numbers::pi_v<float>;

	worldTransformTitle_.translation_.y = std::sin(angle) + 10.0f;

	camera_.TransferMatrix();

	// アフィン変換～DirectXに転送(タイトル座標)
	math_.WorldTransformUpdate(worldTransformTitle_);

	// アフィン変換～DirectXに転送（プレイヤー座標）
	math_.WorldTransformUpdate(worldTransformPlayer_);
}

void TitleScene::Draw() {

	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	modelTitle_->Draw(worldTransformTitle_, camera_);
	modelPlayer_->Draw(worldTransformPlayer_, camera_);

	// フェード
	fade_->Draw();

	Model::PostDraw();

	// --- スプライト描画開始 ---
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// 操作ガイド描画
	if (operationGuide_) {
		operationGuide_->Draw();
	}

	// --- スプライト描画終了 ---
	Sprite::PostDraw();
}

TitleScene::~TitleScene() {
	delete modelPlayer_;
	delete modelTitle_;
	delete fade_;
	delete operationGuide_;
}
