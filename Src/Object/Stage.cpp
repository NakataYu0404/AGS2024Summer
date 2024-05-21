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
	
	
	//	惑星
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

	//	惑星
	for (const auto& s : planets_)
	{
		s.second->Update();
	}

}

void Stage::Draw(void)
{

	//	惑星
	for (const auto& s : planets_)
	{
		s.second->Draw();
	}

}

void Stage::SetObject(std::weak_ptr<Raider> raider, std::weak_ptr<Survivor> survivor1, std::weak_ptr<Survivor> survivor2, std::weak_ptr<Survivor> survivor3 )
{
	raider_ = raider;

	survivor_[0] = survivor1;
	survivor_[1] = survivor2;
	survivor_[2] = survivor3;
}

void Stage::ChangeStage(NAME type)
{

	activeName_ = type;

	//	対象のステージを取得する
	activePlanet_ = GetPlanet(activeName_);

	//	ステージの当たり判定をプレイヤーに設定
	raider_.lock()->ClearCollider();
	raider_.lock()->AddCollider(activePlanet_->GetTransform().lock()->collider);

	for (int i = 0; i < 3; i++)
	{
		survivor_[i].lock()->ClearCollider();
		survivor_[i].lock()->AddCollider(activePlanet_->GetTransform().lock()->collider);
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

	//	最初の惑星
	//------------------------------------------------------------------------------
	std::shared_ptr<Transform> planetTrans;
	planetTrans = std::make_shared<Transform>();

	planetTrans->SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::MAIN_PLANET));
	planetTrans->scl = {2.0f,1.0f,2.0f};
	planetTrans->quaRot = Quaternion();
	planetTrans->pos = { 0.0f, -100.0f, 0.0f };

	//	当たり判定(コライダ)作成
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
