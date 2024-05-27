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
#include "../Object/Player/Survivor.h"
#include "../Object/Mob/Victim.h"
#include "../Object/Planet.h"
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
	
	//	�T�o�C�o�[�쐬
	std::weak_ptr<Transform> raiderTran = raider_->GetTransform();
	std::array<std::weak_ptr<Transform>, SURVIVOR_NUM> surviveTran;

	for (int i = 0; i < survivors_.size(); i++)
	{
		survivors_[i] = std::make_shared<Survivor>(i);
		survivors_[i]->Init();
		surviveTran[i] = survivors_[i]->GetTransform();
		survivors_[i]->SetRaider(raiderTran);
	}

	//	���э쐬(TODO:�Ƃ肠������l������邯�ǁA���Ƃłi�r�n�m�g���Ă����ς��p�ӂ���)	
	std::shared_ptr<Victim> tmp = std::make_shared<Victim>();
	victims_.push_back(tmp);
	for (auto& v : victims_)
	{
		if (v == nullptr)
		{
			break;
		}
		v->Init();
	}

	//	���C�_�[�ɃT�o�C�o�[�A���я���n��
	raider_->SetSurvivor(surviveTran);

	//	�X�e�[�W
	stage_ = std::make_unique<Stage>();
	
	//	�X�e�[�W�ɑ΂��ē����蔻��t����I�u�W�F�N�g��n���񂾂��ǁAsharedptr��z��ɂ����Ⴄ�ƁA�n���Ƃ������weakptr�ɕϊ����Ă���Ȃ��Ȃ����Ⴄ�݂����B
	//	�Ȃ̂ŁA��������weak�^�̔z��Ɉڂ��ւ��āA�����n�����Ƃɂ���
	std::array<std::weak_ptr<Survivor>, SURVIVOR_NUM>tmpSurvivor;
	for (int i = 0; i < survivors_.size(); i++)
	{
		tmpSurvivor[i] = survivors_[i];
	}
	stage_->SetObject(raider_, tmpSurvivor);
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

	for (auto& s : survivors_)
	{
		if (s == nullptr)
		{
			break;
		}
		s->Update();
	}

	for (auto& v : victims_)
	{
		if (v == nullptr)
		{
			break;
		}
		v->Update();
	}
}

void GameScene::Draw(void)
{
	//	�w�i
	skyDome_->Draw();
	stage_->Draw();
	
	raider_->Draw();

	for (auto& s : survivors_)
	{
		if (s == nullptr)
		{
			break;
		}
		s->Draw();
	}

	for (auto& v : victims_)
	{
		if (v == nullptr)
		{
			break;
		}
		v->Draw();
	}

	//	�w���v
	DrawFormatString(840, 20, 0x000000, "�ړ��@�@�FWASD");
	DrawFormatString(840, 40, 0x000000, "�J�����@�F���L�[");
	DrawFormatString(840, 60, 0x000000, "�_�b�V���F�EShift");
	DrawFormatString(840, 80, 0x000000, "�W�����v�F�_(�o�N�X��)");
}
