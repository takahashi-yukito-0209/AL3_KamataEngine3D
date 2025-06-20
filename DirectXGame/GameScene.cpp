#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("sample.png");

	// 3Dモデルの生成
	modelBlock_ = Model::CreateFromOBJ("block", true);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラのfarZを適度に大きい値に変更する
	camera_.farZ = 1280.0f;

	// カメラの初期化
	camera_.Initialize();

	// 自キャラ3Dモデルの生成
	modelPlayer_ = Model::CreateFromOBJ("player", true);

	// マップチップフィールドの生成と初期化
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	// 自キャラの生成
	player_ = new Player();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 18);

	player_->SetMapChipField(mapChipField_);

	// 自キャラの初期化
	player_->Initialize(modelPlayer_, &camera_, playerPosition);

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);
	debugCamera_->SetFarZ(WinApp::kWindowWidth);

	// 3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// 天球の生成と初期化
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	// カメラコントローラ生成
	cameraController_ = new CameraController();

	// カメラコントローラ初期化
	cameraController_->Initialize(&camera_);

	// カメラコントローラ追従対象セット
	cameraController_->SetTarget(player_);

	// カメラコントローラリセット
	cameraController_->Reset();

	// 移動範囲指定
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);

	// 敵キャラ3Dモデルの生成
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);

	// 敵の生成と座標設定、初期化
	for (int32_t i = 0; i < 2; ++i) {

		Enemy* newEnemy = new Enemy();

		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(14 + i * 2, 18);

		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);

		enemies_.push_back(newEnemy);
	}

	// 仮生成処理
	modelDeathParticles_ = Model::CreateFromOBJ("deathParticle", true);
	

	// フェードインから開始
	phase_ = Phase::kFadeIn;

	// フェード初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void GameScene::Update() {

	//フェーズ切り替え
	ChangePhase();

	switch (phase_) {
	case Phase::kFadeIn:
		//フェードの更新
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kPlay;
		}

		// スカイドームの更新
		skydome_->Update();

		// カメラコントローラ更新
		cameraController_->Update();

		// 自キャラの更新
		player_->Update();

		// 敵更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

#ifdef _DEBUG
		// デバックカメラをアクティブにする
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			isDebugCameraActive_ = true;
		}

#endif

		// カメラの処理
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}

				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = math_.MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		
		break;
	case Phase::kPlay:
		// ゲームプレイフェーズの処理

		// 自キャラの更新
		player_->Update();

		// スカイドームの更新
		skydome_->Update();

		// カメラコントローラ更新
		cameraController_->Update();

		// 敵更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

#ifdef _DEBUG
		// デバックカメラをアクティブにする
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			isDebugCameraActive_ = true;
		}

#endif

		// カメラの処理
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}

				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = math_.MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}

		// デバックカメラの更新
		debugCamera_->Update();

		// 衝突判定
		CheckAllCollisions();

		break;

	case Phase::kDeath:
		// デス演出フェーズの処理

		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
		}


		// スカイドームの更新
		skydome_->Update();

		// カメラコントローラ更新
		cameraController_->Update();

		// 敵更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// デスパーティクル更新
		if (deathParticles_) {
			deathParticles_->Update();
		}

#ifdef _DEBUG
		// デバックカメラをアクティブにする
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			isDebugCameraActive_ = true;
		}

#endif

		// カメラの処理
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}

				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = math_.MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}

		
		break;
	case Phase::kFadeOut:
		//フェードの更新
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		// スカイドームの更新
		skydome_->Update();

		// カメラコントローラ更新
		cameraController_->Update();

		// 敵更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}


		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}

				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = math_.MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}

		break;
	}
}

void GameScene::Draw() {

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 自キャラの描画
	if (!player_->IsDead()) {
		player_->Draw();
	}

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}

			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	// スカイドーム描画
	skydome_->Draw();

	// 敵描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	// デスパーティクル描画
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	//フェードの描画
	fade_->Draw();

	// 3Dモデル描画後処理
	Model::PostDraw();
}

void GameScene::GenerateBlocks() {
	// 要素数
	const uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();
	const uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を確保する
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

GameScene::~GameScene() {
	// 3Dモデルデータの解放
	delete modelBlock_;

	// 3Dモデルデータ解放(Player)
	delete modelPlayer_;

	// 自キャラデータの解放
	delete player_;

	// ブロックデータ解放
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	// デバックカメラデータ解放
	delete debugCamera_;

	// 3Dモデルデータ解放(Skydome)
	delete modelSkydome_;

	// マップチップフィールドの解放
	delete mapChipField_;

	// 敵データ解放
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	// デスパーティクルデータ解放
	delete deathParticles_;
	delete modelDeathParticles_;
}

void GameScene::CheckAllCollisions() {
	// 判定対象1と2の座標
	AABB aabb1, aabb2;

#pragma region 自キャラと敵キャラの当たり判定
	{
		// 自キャラの座標
		aabb1 = player_->GetAABB();

		// 自キャラと敵弾全ての当たり判定
		for (Enemy* enemy : enemies_) {
			// 敵弾の座標
			aabb2 = enemy->GetAABB();

			// AABB同士の交差判定
			if (math_.IsCollision(aabb1, aabb2)) {
				// 自キャラの衝突時コールバックを呼び出す
				player_->OnCollision(enemy);
				// 敵弾の衝突時コールバックを呼び出す
				enemy->OnCollision(player_);
			}
		}
	}
#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		// ゲームプレイフェーズの処理

		//自キャラがデス状態
		if (player_->IsDead()) {
			//死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;

			//自キャラの座標を取得
			const KamataEngine::Vector3& deathParticlesPosition = player_->GetWorldPosition();

			//自キャラの座標にデスパーティクルを発生、初期化
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathParticles_, &camera_, deathParticlesPosition);
		}

		break;

	case Phase::kDeath:
		// デス演出フェーズの処理

		break;
	}
}
