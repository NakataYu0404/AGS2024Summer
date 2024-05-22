#include <DxLib.h>
#include "../../Manager/ResourceManager.h"
#include "ShotBase.h"

ShotBase::ShotBase(void)
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

	isAlive_ = false;
}

void ShotBase::SetParam(void)
{
}

void ShotBase::Update(void)
{
}

void ShotBase::Draw(void)
{
	MV1DrawModel(transform_->modelId);
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
