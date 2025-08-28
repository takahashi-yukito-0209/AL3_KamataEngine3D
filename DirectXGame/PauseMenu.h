#pragma once
#include "KamataEngine.h"
#include <input\Input.h>
#include <vector>

class PauseMenu {
public:
	PauseMenu() = default;

	// 初期化
	void Initialize();

	// 更新（EscキーでON/OFF）
	void Update();

	// 描画（PreDraw / PostDrawは外部で行う）
	void Draw();

	// ポーズ状態か取得
	bool IsActive() const { return isActive_; }

	// 選択結果取得
	enum class Result { kNone, kResume, kQuit };
	Result GetResult() const { return result_; }

	// ポーズ状態を外部から変更できるようにする
	void SetActive(bool active) { isActive_ = active; }


private:
	KamataEngine::Vector2 menuPos_ = {400.0f, 200.0f};    // メニュー背景位置
	KamataEngine::Vector2 cursorOffset_ = {-60.0f, 0.0f}; // カーソルのオフセット

	KamataEngine::Sprite* menuBackground_ = nullptr;
	KamataEngine::Sprite* operationGuide_ = nullptr;
	KamataEngine::Sprite* cursor_ = nullptr;

	bool isActive_ = false;
	int selectedIndex_ = 0;
	std::vector<std::string> menuItems_ = {"Resume", "Exit"}; // ← 項目を2つに変更
	Result result_ = Result::kNone;

	KamataEngine::Input* input_ = nullptr;
};
