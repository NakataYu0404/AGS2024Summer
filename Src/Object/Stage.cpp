#include <vector>
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "Player/Raider.h"
#include "Player/Survivor.h"
#include "Planet.h"
#include "Common/Collider.h"
#include "Common/Transform.h"
#include "Stage.h"

Stage::Stage(void)
	: resMng_(ResourceManager::GetInstance())
{

	activeName_ = NAME::MAIN_PLANET;
	step_ = 0.0f;
}

Stage::~Stage(void)
{
	
	
	//	�f��
	for (auto pair : planets_)
	{
		delete pair.second;
	}
	planets_.clear();

}

void Stage::Init(void)
{
	MakeMainStage();

	step_ = -1.0f;
}

void Stage::Update(void)
{

	//	�f��
	for (const auto& s : planets_)
	{
		s.second->Update();
	}

}

void Stage::Draw(void)
{

	//	�f��
	for (const auto& s : planets_)
	{
		s.second->Draw();
	}

}

void Stage::SetObject(std::weak_ptr<Raider> raider, std::array<std::weak_ptr<Survivor>, Raider::SURVIVOR_NUM> survivor)
{
	raider_ = raider;

	survivors_ = survivor;
}

void Stage::ChangeStage(NAME type)
{

	activeName_ = type;

	//	�Ώۂ̃X�e�[�W���擾����
	activePlanet_ = GetPlanet(activeName_);

	//	�X�e�[�W�̓����蔻����v���C���[�ɐݒ�
	raider_.lock()->ClearCollider();
	raider_.lock()->AddCollider(activePlanet_->GetTransform().lock()->collider);

	for (int i = 0; i < 3; i++)
	{
		survivors_[i].lock()->ClearCollider();
		survivors_[i].lock()->AddCollider(activePlanet_->GetTransform().lock()->collider);
	}

	step_ = TIME_STAGE_CHANGE;

}

Planet* Stage::GetPlanet(NAME type)
{
	if (planets_.count(type) == 0)
	{
		return nullPlanet;
	}

	return planets_[type];
}

void Stage::MakeMainStage(void)
{

	//	�ŏ��̘f��
	//------------------------------------------------------------------------------
	std::shared_ptr<Transform> planetTrans;
	planetTrans = std::make_shared<Transform>();

	planetTrans->SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::MDL_MAINPLANET));
	planetTrans->scl = {2.0f,1.0f,2.0f};
	planetTrans->quaRot = Quaternion();
	planetTrans->pos = { 0.0f, -100.0f, 0.0f };

	//	�����蔻��(�R���C�_)�쐬
	planetTrans->MakeCollider(Collider::TYPE::STAGE);

	planetTrans->Update();

	NAME name = NAME::MAIN_PLANET;
	Planet* planet =
		new Planet(
			name, Planet::TYPE::GROUND, planetTrans);
	planet->Init();
	planets_.emplace(name, planet);
	//------------------------------------------------------------------------------

}
