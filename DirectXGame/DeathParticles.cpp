#include "DeathParticles.h"

void DeathParticles::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	for (auto& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	//オブジェクト初期化
	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};
}

void DeathParticles::Update() {

	//終了なら何もしない
	if (isFinished_) {
		return;
	}

	//カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	//存続時間の上限に達したら
	if (counter_ >= kDuration_) {
		counter_ = kDuration_;
		//終了扱いにする
		isFinished_ = true;
	}

	for (uint32_t i = 0; i < kNumParticles; i++) {
		// 基本となる速度ベクトル
		KamataEngine::Vector3 velocity = {kSpeed_, 0, 0};

		// 回転角を計算する
		float angle = kAngleUnit_ * i;

		// Z軸まわり回転行列
		KamataEngine::Matrix4x4 matrixRotation = math_.MakeRotateZMatrix(angle);

		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = math_.Transform(velocity, matrixRotation);

		// 移動処理
		worldTransforms_[i].translation_ += velocity;
	}

	
	color_.w = std::clamp(1.0f - counter_ / kDuration_, 0.0f, 1.0f);
	// 色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);


	for (auto& worldTransform : worldTransforms_) {
		// アフィン変換とバッファ転送を一括でする関数
		math_.WorldTransformUpdate(worldTransform);
	}
}

void DeathParticles::Draw() {

	// 終了なら何もしない
	if (isFinished_) {
		return;
	}

	// 3Dモデルを描画
	for (auto& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_,&objectColor_);
	}
}
