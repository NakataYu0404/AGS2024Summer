#include <DxLib.h>
#include "../../Manager/ResourceManager.h"
#include "../Common/Collider.h"
#include "../Common/CollisionManager.h"
#include "../Common/Sphere.h"
#include "ShotBase.h"

ShotBase::ShotBase(void):colMng_(CollisionManager::GetInstance())
{

}

ShotBase::~ShotBase(void)
{

}

void ShotBase::Init(void)
{
	transform_ = std::make_shared<Transform>();

	transform_->SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::SHOT));
	transform_->pos = { 0.0f,0.0f,0.0f };
	transform_->scl = { 1.0f,1.0f,1.0f };
	transform_->quaRot = Quaternion();
	transform_->quaRotLocal =
		Quaternion::Euler({ 0.0f, 0.0f, 0.0f });
	transform_->Update();

	transform_->MakeCollider(Collider::Category::SHOT, Collider::TYPE::SPHERE);

	SetParam();

	//	カプセルコライダ
	sphere_ = std::make_shared<Sphere>(transform_);
	sphere_->SetLocalPos({ 0.0f, 0.0f, 0.0f });
	sphere_->SetRadius(20.0f);
}

void ShotBase::SetParam(void)
{
	isAlive_ = false;
	moveDir_ = { 0.0f,0.0f,0.0f };
	speed_ = 80.0f;
}

void ShotBase::Update(void)
{
	if (!isAlive_)
	{
		return;
	}
	transform_->pos = VAdd(transform_->pos, VScale(moveDir_, speed_));
	transform_->Update();
}

void ShotBase::Draw(void)
{
	if (!isAlive_)
	{
		return;
	}
	MV1DrawModel(transform_->modelId);
	sphere_->Draw();
}

VECTOR ShotBase::GetPos(void)
{
	return transform_->pos;
}

bool ShotBase::IsAlive(void)
{
	return isAlive_;
}

void ShotBase::SetPos(VECTOR pos)
{
	transform_->pos = pos;
	transform_->Update();
}

void ShotBase::SetDir(VECTOR dir)
{
	moveDir_ = dir;
	transform_->Update();
}

void ShotBase::SetAlive(bool isAlive)
{
	isAlive_ = isAlive;
}

void ShotBase::OnCollision(std::weak_ptr<Collider> collider)
{
	//	何に当たっても消える


	//	他のクラスにこのクラスから影響させるのは避ける(余計な参照権を与えない)
	auto cate = collider.lock()->category_;
	switch (cate)
	{
	case Collider::Category::RAIDER:
		
		break;
	case Collider::Category::SURVIVOR:
		SetAlive(false);

		break;
	case Collider::Category::STAGE:
		SetAlive(false);

		break;
	case Collider::Category::SHOT:

		break;
	default:
		break;
	}

}
