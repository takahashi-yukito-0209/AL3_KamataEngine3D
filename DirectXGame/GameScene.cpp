#include "GameScene.h"
#include <random>

using namespace KamataEngine;

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("sample.png");

	// 3Dモデルの生成
	modelBlock_ = Model::CreateFromOBJ("block", true);

	// 3Dモデルの生成
	modelGoal_ = Model::CreateFromOBJ("goal", true);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラのfarZを適度に大きい値に変更する
	camera_.farZ = 1280.0f;

	// カメラの初期化
	camera_.Initialize();

	// 自キャラ3Dモデルの生成
	modelPlayer_ = Model::CreateFromOBJ("player", true);

	// プレイヤー攻撃用モデルの生成
	modelAttack_ = Model::CreateFromOBJ("attack_effect", true);

    // マップチップフィールドの生成
    mapChipField_ = new MapChipField();

    // ステージファイル一覧を用意（必要なCSVをここに追加）
    stageFiles_.push_back("Resources/blocks.csv");
    stageFiles_.push_back("Resources/stage2.csv");

    // 初期ステージを読み込む
    LoadStage(0);

	// 自キャラの生成
	player_ = new Player();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 18);

	player_->SetMapChipField(mapChipField_);

	// 自キャラの初期化
	player_->Initialize(modelPlayer_, modelAttack_, &camera_, playerPosition);

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

    // （射撃物や敵の攻撃エフェクト用モデルは削除済み）

    // 敵の生成と座標設定、初期化（ランダムスポーン）
    {
        // ランダム生成の準備
        std::random_device rd;
        std::mt19937 mt(rd());

        auto spawnRandom = [&](Enemy::Pattern pattern, int count) {
            const uint32_t w = mapChipField_->GetNumBlockHorizontal();
            const uint32_t h = mapChipField_->GetNumBlockVirtical();
            std::uniform_int_distribution<int> distX(0, static_cast<int>(w) - 1);
            std::uniform_int_distribution<int> distY(0, static_cast<int>(h) - 1);

            int attemptsLimit = 500;
            for (int n = 0; n < count; ++n) {
                Vector3 pos;
                bool found = false;
                for (int attempt = 0; attempt < attemptsLimit; ++attempt) {
                    int xi = distX(mt);
                    int yi = distY(mt);
                    // スポーンタイルは空白であることを要求
                    if (mapChipField_->GetMapChipTypeByIndex(xi, yi) != MapChipType::kBlank) continue;

                    pos = mapChipField_->GetMapChipPositionByIndex(xi, yi);
                    // 少しだけ浮かせる
                    pos.y += 0.2f;

                    // プレイヤーと被らないように近接チェック
                    if (player_) {
                        Vector3 ppos = player_->GetWorldPosition();
                        float dx = pos.x - ppos.x;
                        float dy = pos.y - ppos.y;
                        if (std::sqrt(dx * dx + dy * dy) < 1.5f) {
                            continue; // 近すぎるのでリジェクト
                        }
                    }

                    found = true;
                    break;
                }

                if (!found) {
                    // 見つからなければ既存の中心付近にフォールバック
                    pos = mapChipField_->GetMapChipPositionByIndex(w / 2, h / 2);
                    pos.y += 0.2f;
                }

                Enemy* e = new Enemy();
                e->Initialize(modelEnemy_, &camera_, pos);
                e->SetGameScene(this);
                e->SetPattern(pattern);
                e->SetTarget(player_);

                // パトロールなら左右範囲を設定
                if (pattern == Enemy::Pattern::kPatrol) {
                    e->SetPatrolRange(pos.x - 2.0f, pos.x + 2.0f);
                }

                enemies_.push_back(e);
            }
        };

        // パターンごとに生成数を指定
        spawnRandom(Enemy::Pattern::kPatrol, 3);
        spawnRandom(Enemy::Pattern::kChase, 2);
        spawnRandom(Enemy::Pattern::kStop, 2);
        spawnRandom(Enemy::Pattern::kFly, 2);
    }

    // （逃走する敵は無し）

	// 仮生成処理
	modelDeathParticles_ = Model::CreateFromOBJ("deathParticle", true);

	// フェードインから開始
	phase_ = Phase::kFadeIn;

	// フェード初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// ヒットエフェクト用モデルの読み込み
	modelEffect_ = Model::CreateFromOBJ("plane", true);
	HitEffect::SetModel(modelEffect_);
	HitEffect::SetCamera(&camera_);
}

void GameScene::Update() {

	// フェーズ切り替え
	ChangePhase();

	switch (phase_) {
	case Phase::kFadeIn:
	{
		// フェードの更新
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
		// プレイヤーが死んだ or ゴール到達(クリア) のときは敵の動きを止める
		// 敵を停止させるのはプレイヤーが死亡したとき、またはクリア時
		bool stopEnemies = (player_ && (player_->IsDead() || player_->HasReachedGoal()));
		for (Enemy* enemy : enemies_) {
			if (stopEnemies) {
				// プレイヤー死亡・クリア時は移動を止めて浮遊アニメーションのみ行う
				enemy->BehaviorStopUpdate();
			} else {
				enemy->Update();
			}
			// 常にステージ内に収める補正は行う
			enemy->KeepWithinStage(mapChipField_);
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
	}
	case Phase::kPlay:
	{
		// ゲームプレイフェーズの処理

		// 自キャラの更新
		player_->Update();

		// スカイドームの更新
		skydome_->Update();

		// カメラコントローラ更新
		cameraController_->Update();

		// 敵更新
		// プレイヤーが死んだ or ゴール到達(クリア) のときは敵の動きを止める
		// 敵を停止させるのはプレイヤーが死亡したとき、またはクリア時
		bool stopEnemies = (player_ && (player_->IsDead() || player_->HasReachedGoal()));
		for (Enemy* enemy : enemies_) {
			if (stopEnemies) {
				// プレイヤー死亡・クリア時は移動を止めて浮遊アニメーションのみ行う
				enemy->BehaviorStopUpdate();
			} else {
				enemy->Update();
			}
			// 常にステージ内に収める補正は行う
			enemy->KeepWithinStage(mapChipField_);
		}

		// ヒットエフェクト描画
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
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
	}

	case Phase::kDeath:
	{
		// デス演出フェーズの処理

		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
		}

		// スカイドームの更新
		skydome_->Update();

		// カメラコントローラ更新
		cameraController_->Update();

		// 敵更新
		// プレイヤーが死んだ or ゴール到達(クリア) のときは敵の動きを止める
		// 敵を停止させるのはプレイヤーが死亡したとき、またはクリア時
		bool stopEnemies = (player_ && (player_->IsDead() || player_->HasReachedGoal()));
		for (Enemy* enemy : enemies_) {
			if (stopEnemies) {
				// プレイヤー死亡・クリア時は移動を止めて浮遊アニメーションのみ行う
				enemy->BehaviorStopUpdate();
			} else {
				enemy->Update();
			}
			// 常にステージ内に収める補正は行う
			enemy->KeepWithinStage(mapChipField_);
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
	}
	case Phase::kFadeOut:
	{
		// フェードの更新
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		// スカイドームの更新
		skydome_->Update();

		// カメラコントローラ更新
		cameraController_->Update();

		// 敵更新
		// プレイヤーが死んだ or ゴール到達(クリア) のときは敵の動きを止める
		// 敵を停止させるのはプレイヤーが死亡したとき、またはクリア時
		bool stopEnemies = (player_ && (player_->IsDead() || player_->HasReachedGoal()));
		for (Enemy* enemy : enemies_) {
			if (stopEnemies) {
				// プレイヤー死亡・クリア時は移動を止めて浮遊アニメーションのみ行う
				enemy->BehaviorStopUpdate();
			} else {
				enemy->Update();
			}
			// 常にステージ内に収める補正は行う
			enemy->KeepWithinStage(mapChipField_);
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

	// デスフラグの立った敵を削除
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	// 死んだプロジェクタイルの削除
    // プロジェクタイル無効化: ランタイムでの弾管理は行っていない
}

void GameScene::Draw() {

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());


	// ブロックの描画
	for (uint32_t i = 0; i < worldTransformBlocks_.size(); ++i) {
		for (uint32_t j = 0; j < worldTransformBlocks_[i].size(); ++j) {
			WorldTransform* worldTransformBlock = worldTransformBlocks_[i][j];
			if (!worldTransformBlock) {
				continue;
			}

			// マップチップ種別を取得
			MapChipType chipType = mapChipField_->GetMapChipTypeByIndex(j, i);

			// ブロックの種類によって描画モデルを切り替え
			if (chipType == MapChipType::kBlock) {
				modelBlock_->Draw(*worldTransformBlock, camera_);
			} else if (chipType == MapChipType::kGoal) {
				modelGoal_->Draw(*worldTransformBlock, camera_); 
			}
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

	//ヒットエフェクト描画
	for (HitEffect* hitEffect : hitEffects_) {
		hitEffect->Draw();
	}

	// 自キャラの描画
	if (!player_->IsDead()) {
		player_->Draw();
	}

	// フェードの描画
	fade_->Draw();

	// 3Dモデル描画後処理
	Model::PostDraw();
}

void GameScene::GenerateBlocks() {
	// 要素数
	const uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();
	const uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

    // 以前のブロックデータを解放してから再構築
    for (std::vector<WorldTransform*>& line : worldTransformBlocks_) {
        for (WorldTransform* wt : line) {
            delete wt;
        }
    }
    worldTransformBlocks_.clear();

    // 要素数を確保する
    worldTransformBlocks_.resize(kNumBlockVirtical);
    for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
        worldTransformBlocks_[i].resize(kNumBlockHorizontal);
    }

    // キューブの生成
    for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
        for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {

            MapChipType chipType = mapChipField_->GetMapChipTypeByIndex(j, i);

            if (chipType == MapChipType::kBlock || chipType == MapChipType::kGoal) {
                WorldTransform* worldTransform = new WorldTransform();
                worldTransform->Initialize();
                worldTransformBlocks_[i][j] = worldTransform;
                worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
            }
        }
    }
}

void GameScene::LoadStage(int stageIndex) {
    if (stageIndex < 0 || stageIndex >= static_cast<int>(stageFiles_.size())) {
        return;
    }

    currentStageIndex_ = stageIndex;

    // CSV読み込み
    mapChipField_->LoadMapChipCsv(stageFiles_[stageIndex]);

    // ブロック再生成
    GenerateBlocks();

    // プレイヤーと敵の初期化位置はCSVに依存するため、簡易的にプレイヤーを最初のブロック近辺に配置
    if (player_) {
        // 仮として(2,18)の位置を使用
        KamataEngine::Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 18);
        player_->SetPosition(playerPosition);
    }
}

void GameScene::CreateHitEffect(const KamataEngine::Vector3 position) { 
	HitEffect* newHitEffect = HitEffect::Create(position);
	hitEffects_.push_back(newHitEffect);
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

	//ヒットエフェクトの解放
	for (HitEffect* hitEffect : hitEffects_) {
		delete hitEffect;
	}

	//ヒットエフェクト用モデルの解放
	delete modelEffect_;

	// プロジェクタイルの解放
	// projectiles removed

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
			if (enemy->IsCollisionDisabled()) {
				continue; // コリジョン無効の敵はスキップ
			}

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

		// 自キャラがゴールに到達した場合
		if (player_->HasReachedGoal()) {
			phase_ = Phase::kFadeOut;                
			fade_->Start(Fade::Status::FadeOut, 1.0f); // フェード開始
		}

		// 自キャラがデス状態
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;

			// 自キャラの座標を取得
			const KamataEngine::Vector3& deathParticlesPosition = player_->GetWorldPosition();

			// 自キャラの座標にデスパーティクルを発生、初期化
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathParticles_, &camera_, deathParticlesPosition);
		}

		break;

	case Phase::kDeath:
		// デス演出フェーズの処理

		break;
	}
}
