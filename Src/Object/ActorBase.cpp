#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "ActorBase.h"

ActorBase::ActorBase(void)
	: resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance())
{
}

ActorBase::~ActorBase(void)
{
	transform_->Release();
}

void ActorBase::SetParam(void)
{
}

const std::weak_ptr<Transform> ActorBase::GetTransform(void) const
{
	return transform_;
}

const std::weak_ptr<Capsule> ActorBase::GetCapsule(void) const
{
	return capsule_;
}
