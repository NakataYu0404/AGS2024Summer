#include <Dxlib.h>
#include "../../Utility/AsoUtility.h"
#include "../../Manager/ResourceManager.h"
#include "../Common/AnimationController.h"
#include "Victim.h"

Victim::Victim()
{
}

Victim::~Victim()
{
}

void Victim::Init()
{
	transform_ = std::make_shared<Transform>();

	//	���f���̊�{�ݒ�
	transform_->SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::MDL_PLAYER));
	transform_->scl = AsoUtility::VECTOR_ONE;
	transform_->pos = { 0.0f, -30.0f, 1000.0f };
	transform_->quaRot = Quaternion();
	transform_->quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_->Update();

	//	�ۉe�摜
	imgShadow_ = resMng_.Load(ResourceManager::SRC::IMG_PLAYERSHADOW).handleId_;

	//	�������
	ChangeState(STATE::PLAY);

}

void Victim::Update()
{
	//	�X�V�X�e�b�v
	switch (state_)
	{
	case Victim::STATE::NONE:
		UpdateNone();
		break;
	case Victim::STATE::PLAY:
		UpdatePlay();
		break;
	}
	//	���f������X�V
	transform_->Update();

	//	//	�A�j���[�V�����Đ�
	//	animationController_->Update();

}

void Victim::Draw()
{
	//	���f���̕`��
	MV1DrawModel(transform_->modelId);

	//	�ۉe�`��
	DrawShadow();

}

void Victim::SetParam()
{
}

void Victim::InitAnimation(void)
{
}

void Victim::ChangeState(STATE state)
{
	state_ = state;
}

void Victim::ChangeStateNone(void)
{
}

void Victim::ChangeStatePlay(void)
{
}

void Victim::UpdateNone(void)
{
}

void Victim::UpdatePlay(void)
{
}

void Victim::DrawShadow(void)
{
}
