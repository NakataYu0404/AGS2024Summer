#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/SkyDome.h"
#include "../Object/Stage.h"
#include "../Object/Player/Raider.h"
#include "../Object/Planet.h"
#include "../Object/Player/Survivor.h"
#include "GameScene.h"

GameScene::GameScene(void)
{
	skyDome_ = nullptr;
	stage_ = nullptr;
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	//	���C�_�[
	raider_ = std::make_shared<Raider>();
	raider_->Init();
	
	//	�T�o�C�o�[
	survivor_ = std::make_shared<Survivor>();
	survivor_->Init();

	raider_->SetEnemy(survivor_->GetTransform());
	survivor_->SetEnemy(raider_->GetTransform());

	//	�X�e�[�W
	stage_ = std::make_unique<Stage>(raider_, survivor_);
	stage_->Init();

	//	�X�e�[�W�̏����ݒ�
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	//	�X�J�C�h�[��
	skyDome_ = std::make_unique<SkyDome>();
	skyDome_->Init();



	SceneManager::GetInstance().GetCamera()->SetFollow(raider_->GetTransform().lock().get());
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FOLLOW);
}

void GameScene::Update(void)
{
	//	�V�[���J��
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_NUMPAD0))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	skyDome_->Update();

	stage_->Update();

	raider_->Update();
	survivor_->Update();
}

void GameScene::Draw(void)
{

	//	�w�i
	skyDome_->Draw();
	stage_->Draw();
	
	raider_->Draw();
	survivor_->Draw();

	//	�w���v
	DrawFormatString(840, 20, 0x000000, "�ړ��@�@�FWASD");
	DrawFormatString(840, 40, 0x000000, "�J�����@�F���L�[");
	DrawFormatString(840, 60, 0x000000, "�_�b�V���F�EShift");
	DrawFormatString(840, 80, 0x000000, "�W�����v�F�_(�o�N�X��)");
}
