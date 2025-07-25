#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("sample.png");

	// 3Dモデルの生成
	model_ = Model::CreateFromOBJ("cube", true);

	//自キャラの生成
	player_ = new Player();
	//自キャラの初期化
	player_->Initialize(model_,textureHandle_);

	// カメラのfarZを適度に大きい値に変更する
	camera_.farZ = 1280.0f;

	// カメラの初期化
	camera_.Initialize();
}

void GameScene::Update() {
	//自キャラの更新
	player_->Update();

}

void GameScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());
	
	// 自キャラの描画
	player_->Draw(&camera_);

	// 3Dモデル描画後処理
	Model::PostDraw();
}

GameScene::~GameScene() {
	// 3Dモデルデータの解放
	delete model_;

	//自キャラの解放
	delete player_;
}
