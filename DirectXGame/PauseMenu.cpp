#include "PauseMenu.h"
#include <base\DirectXCommon.h>
#include <base\TextureManager.h>

using namespace KamataEngine;

void PauseMenu::Initialize() {
	input_ = Input::GetInstance();

	// 背景スプライト（512x512 → 300x300 に縮小）
	uint32_t bgHandle = TextureManager::Load("pause_menu.png");
	menuBackground_ = Sprite::Create(bgHandle, menuPos_);
	menuBackground_->SetSize({300.0f, 300.0f});

	// 操作ガイドスプライト（512x512 → 250x250 に縮小、右下へ表示）
	uint32_t guideHandle = TextureManager::Load("operation_guide.png");
	operationGuide_ = Sprite::Create(guideHandle, {1000.0f, 460.0f});
	operationGuide_->SetSize({250.0f, 250.0f});

	// カーソルスプライト（100x100 → 32x32 に縮小）
	uint32_t cursorHandle = TextureManager::Load("cursor.png");
	cursorOffset_ = {40.0f, 90.0f}; // ← テキストの左横に自然に来るよう調整
	cursor_ = Sprite::Create(cursorHandle, {cursorOffset_.x, cursorOffset_.y});
	cursor_->SetSize({32.0f, 32.0f});
}

void PauseMenu::Update() {
	// EscキーでポーズON/OFF切り替え
	if (input_->TriggerKey(DIK_ESCAPE)) {
		isActive_ = !isActive_;
		result_ = Result::kNone;
	}

	if (!isActive_)
		return;

	// 上下で選択
	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W)) {
		selectedIndex_--;
		if (selectedIndex_ < 0) {
			selectedIndex_ = int(menuItems_.size() - 1);
		}
	}
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S)) {
		selectedIndex_++;
		if (selectedIndex_ >= int(menuItems_.size())) {
			selectedIndex_ = 0;
		}
	}

	// 決定
	if (input_->TriggerKey(DIK_SPACE)) {
		if (selectedIndex_ == 0)
			result_ = Result::kResume;
		else if (selectedIndex_ == 1)
			result_ = Result::kQuit;
	}
}

void PauseMenu::Draw() {
	if (!isActive_)
		return;

	// 背景描画
	menuBackground_->Draw();

	// 操作ガイド描画
	operationGuide_->Draw();

	// カーソル位置をメニューに合わせて更新
	Vector2 cursorPos = menuPos_;
	cursorPos.y += selectedIndex_ * 70.0f + cursorOffset_.y; // offset を足す
	cursorPos.x += cursorOffset_.x;
	cursor_->SetPosition(cursorPos);

	// カーソル描画
	cursor_->Draw();
}
