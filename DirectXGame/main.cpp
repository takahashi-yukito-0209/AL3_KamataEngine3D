#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>

using namespace KamataEngine;

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;

//シーン(型)
enum class Scene { 
	kUnknown = 0,
	kTitle,
	kGame,
};

//現在シーン(型)
Scene scene = Scene::kUnknown;

//シーン切り替え関数(グローバル)
void ChangeScene() {
	
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			//シーン変更
			scene = Scene::kGame;

			//旧シーンの解放
			delete titleScene;
			titleScene = nullptr;

			//新シーンの生成と初期化
			gameScene = new GameScene();
			gameScene->Initialize();
		}

		break;

	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;

			// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;

			// 新シーンの生成と初期化
			titleScene = new TitleScene();
			titleScene->Initialize();
		}

		break;

	}
}

//シーンの更新関数(グローバル)
void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();

		break;

	case Scene::kGame:
		gameScene->Update();
		
		break;

	}
}

// シーンの描画関数(グローバル)
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();

		break;

	case Scene::kGame:
		gameScene->Draw();

		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"LE2C_17_タカハシ_ユキト_AL3");

	//最初のシーンの初期化
	scene = Scene::kTitle;
	titleScene = new TitleScene();
	titleScene->Initialize();

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// メインループ
	while (true) {

		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		//シーン切り替え
		ChangeScene();

		//現在シーン更新
		UpdateScene();

		// 描画開始
		dxCommon->PreDraw();

		//現在シーンの描画
		DrawScene();

		// 描画終了
		dxCommon->PostDraw();
	}

	//シーン解放処理
	delete titleScene;
	delete gameScene;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}
