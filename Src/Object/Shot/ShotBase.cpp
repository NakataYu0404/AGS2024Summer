#include <DxLib.h>
#include "ShotBase.h"

ShotBase::ShotBase(void)
{

}

ShotBase::~ShotBase(void)
{
}

void ShotBase::Init(void)
{
	transform_->pos = { 0.0f,0.0f,0.0f };
	transform_->scl = { 1.0f,1.0f,1.0f };
	
}

void ShotBase::SetParam(void)
{
}

void ShotBase::Update(void)
{
}

void ShotBase::Draw(void)
{
}
