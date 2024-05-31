#include <vector>
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "Player/Raider.h"
#include "Player/Survivor.h"
#include "Common/CollisionManager.h"
#include "Common/Collider.h"
#include "Common/Transform.h"
#include "Stage.h"

Stage::Stage(void)
{

}

Stage::~Stage(void)
{
	
}

void Stage::Init(void)
{
	transform_ = std::make_shared<Transform>();

	transform_->SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::MDL_MAINPLANET));
	transform_->scl = {2.0f,1.0f,2.0f};
	transform_->quaRot = Quaternion();
	transform_->pos = { 0.0f, -100.0f, 0.0f };

	//	“–‚½‚è”»’è(ƒRƒ‰ƒCƒ_)ì¬
	transform_->MakeCollider(Collider::Category::STAGE,Collider::TYPE::MODEL);

	transform_->Update();

}

void Stage::Update(void)
{

}

void Stage::Draw(void)
{
	MV1DrawModel(transform_->modelId);
}

void Stage::SetObject(std::weak_ptr<Raider> raider, std::array<std::weak_ptr<Survivor>, Raider::SURVIVOR_NUM> survivor)
{
	raider_ = raider;

	survivors_ = survivor;
}
