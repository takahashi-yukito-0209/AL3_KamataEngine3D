#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	//�t�@�C������w�肵�ăe�N�X�`����ǂݍ���
	textureHandle_ = TextureManager::Load("sample.png");

	//3D���f���̐���
	model_ = Model::Create();

	//���[���h�g�����X�t�H�[���̏�����
	worldTransform_.Initialize();

	//�J�����̏�����
	camera_.Initialize();

	//���L�����̐���
	player_ = new Player();

	//���L�����̏�����
	player_->Initialize(model_,textureHandle_,&camera_);

}

void GameScene::Update() {
	//���L�����̍X�V
	player_->Update();
}

void GameScene::Draw() {

	//DirectXCommon�C���X�^���X�̎擾
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//3D���f���`��O����
	Model::PreDraw(dxCommon->GetCommandList());

	// ���L�����̕`��
	player_->Draw();

	//3D���f���`��㏈��
	Model::PostDraw();
}

GameScene::~GameScene() {
	delete model_;
	delete player_;
}
