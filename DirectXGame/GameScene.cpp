#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("sample.png");

	// 3Dモデルの生成
	model_ = Model::CreateFromOBJ("cube", true);

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_);

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// カメラのfarZを適度に大きい値に変更する
	camera_.farZ = 1280.0f;

	// カメラの初期化
	camera_.Initialize();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	//軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetCamera(&camera_);

}

void GameScene::Update() {
	// 自キャラの更新
	player_->Update();


#ifdef _DEBUG
	
	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true;
	}

	//カメラの処理
	if (isDebugCameraActive_) {
		//  デバッグカメラの更新
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		//ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}

#endif
}

void GameScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 自キャラの描画
	player_->Draw(&camera_);

	//軸の描画
	AxisIndicator::GetInstance()->Draw();

	// 3Dモデル描画後処理
	Model::PostDraw();
}

GameScene::~GameScene() {
	// 3Dモデルデータの解放
	delete model_;

	// 自キャラの解放
	delete player_;

	// デバックカメラの解放
	delete debugCamera_;
}
