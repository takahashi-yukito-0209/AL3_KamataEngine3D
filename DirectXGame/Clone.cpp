#include "Clone.h"

// --------------------------------------------
// 初期化
// --------------------------------------------
void Clone::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	model_ = model;
	camera_ = camera;

	// ワールド変換を初期化
	worldTransform_.Initialize();

	// とりあえずプレイヤーの横に配置
	worldTransform_.translation_ = {2.0f, 0, 0};
}

// --------------------------------------------
// 更新
// playerPos … 毎フレームのプレイヤー座標
// --------------------------------------------
void Clone::Update(const KamataEngine::Vector3& playerPos) {
	switch (mode_) {
	case Mode::kNone:
		// 静止
		break;

	case Mode::kPlayback:
		// 過去の履歴を順に再生
		if (!history_.empty()) {
			worldTransform_.translation_ = history_[currentIndex_];
			currentIndex_ = (currentIndex_ + 1) % history_.size();
		}
		break;

	case Mode::kRewind:
		// 過去の履歴を逆順に再生
		if (!history_.empty()) {
			worldTransform_.translation_ = history_[currentIndex_];
			if (currentIndex_ > 0) {
				currentIndex_--;
			} else {
				currentIndex_ = history_.size() - 1;
			}
		}
		break;
	}

	// プレイヤー座標を履歴に追加
	history_.push_back(playerPos);

	// 古いデータを削除（最大300フレーム分保持）
	if (history_.size() > 300) {
		history_.erase(history_.begin());
	}

	// 行列更新
	math_.WorldTransformUpdate(worldTransform_);
}

// --------------------------------------------
// 描画
// --------------------------------------------
void Clone::Draw(KamataEngine::Camera* camera) {
	if (model_) {
		model_->Draw(worldTransform_, *camera);
	}
}
