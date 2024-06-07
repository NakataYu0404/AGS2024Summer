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

	//	モデルの基本設定
	transform_->SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::MDL_PLAYER));
	transform_->scl = AsoUtility::VECTOR_ONE;
	transform_->pos = { 0.0f, -30.0f, 1000.0f };
	transform_->quaRot = Quaternion();
	transform_->quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_->Update();

	//	丸影画像
	imgShadow_ = resMng_.Load(ResourceManager::SRC::IMG_PLAYERSHADOW).handleId_;

	//	初期状態
	ChangeState(STATE::PLAY);

}

void Victim::Update()
{
	//	更新ステップ
	switch (state_)
	{
	case Victim::STATE::NONE:
		UpdateNone();
		break;
	case Victim::STATE::PLAY:
		UpdatePlay();
		break;
	}
	//	モデル制御更新
	transform_->Update();

	//	//	アニメーション再生
	//	animationController_->Update();

}

void Victim::Draw()
{
	//	モデルの描画
	MV1DrawModel(transform_->modelId);

	//	丸影描画
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
