#include "PlayerBase.h"
#include <string>
#include <DxLib.h>
#include "../../Application.h"
#include "../../Utility/AsoUtility.h"
#include "../../Manager/InputManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Camera.h"
#include "../Common/AnimationController.h"
#include "../Common/Capsule.h"
#include "../Common/Collider.h"
#include "../Common/CollisionManager.h"
#include "../Shot/ShotBase.h"
#include "PlayerBase.h"

PlayerBase::PlayerBase(void) : colMng_(CollisionManager::GetInstance())
{
	SetParam();
}

PlayerBase::~PlayerBase(void)
{
}

void PlayerBase::AddCollider(std::shared_ptr<Collider> collider)
{
	colliders_.push_back(collider);
}

void PlayerBase::ClearCollider(void)
{
	colliders_.clear();
}


void PlayerBase::AddCapsule(std::shared_ptr<Capsule> capsule)
{
}

float PlayerBase::GetGravity(void)
{
	return gravityPow_;
}

void PlayerBase::SetBlowOff(VECTOR vec, float pow, float stunTime)
{
	BlowOff();
	blowOffVec_ = vec;
	blowOffPow_ = pow;
	stunTime_ = stunTime;
}

void PlayerBase::Damage(int damage)
{
	hp_ -= damage;

	if (hp_ <= 0)
	{
		KnockOuted();
	}

}

void PlayerBase::ChangeState(STATE state)
{
	//	��ԕύX
	state_ = state;

	//	�e��ԑJ�ڂ̏�������
	switch (state_)
	{
	case PlayerBase::STATE::NONE:
		ChangeStateNone();
		break;
	case PlayerBase::STATE::PLAY:
		ChangeStatePlay();
		break;
	}
}

void PlayerBase::ChangeStateNone(void)
{
}

void PlayerBase::ChangeStatePlay(void)
{
}

void PlayerBase::UpdateNone(void)
{
}

void PlayerBase::ShotDestinationPosUpdate(void)
{
	cameraMidRayPos_ = SceneManager::GetInstance().GetCamera()->GetMidRayDestantionPos();

	auto Hit = colMng_.Line_IsCollision_Stage(SceneManager::GetInstance().GetCamera()->GetPos(), cameraMidRayPos_);

	if (Hit.HitFlag)
	{
		shotDestinationPos_ = Hit.HitPosition;
	}
	else
	{
		shotDestinationPos_ = cameraMidRayPos_;
	}

}

void PlayerBase::DrawShadow(void)
{

	float PLAYER_SHADOW_HEIGHT = 300.0f;
	float PLAYER_SHADOW_SIZE = 30.0f;

	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3] = { VERTEX3D(), VERTEX3D(), VERTEX3D() };
	VECTOR SlideVec;
	int ModelHandle;

	//	���C�e�B���O�𖳌��ɂ���
	SetUseLighting(FALSE);

	//	�y�o�b�t�@��L���ɂ���
	SetUseZBuffer3D(TRUE);

	//	�e�N�X�`���A�h���X���[�h�� CLAMP �ɂ���( �e�N�X�`���̒[����͒[�̃h�b�g�����X���� )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	//	�e�𗎂Ƃ����f���̐������J��Ԃ�
	for (const auto c : colliders_)
	{

		//	�`�F�b�N���郂�f���́Aj��0�̎��̓X�e�[�W���f���A1�ȏ�̏ꍇ�̓R���W�������f��
		ModelHandle = c->modelId_;

		//	�v���C���[�̒����ɑ��݂���n�ʂ̃|���S�����擾
		HitResDim = MV1CollCheck_Capsule(
			ModelHandle, -1,
			transform_->pos, VAdd(transform_->pos, { 0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f }), PLAYER_SHADOW_SIZE);

		//	���_�f�[�^�ŕω��������������Z�b�g
		Vertex[0].dif = GetColorU8(255, 255, 255, 255);
		Vertex[0].spc = GetColorU8(0, 0, 0, 0);
		Vertex[0].su = 0.0f;
		Vertex[0].sv = 0.0f;
		Vertex[1] = Vertex[0];
		Vertex[2] = Vertex[0];

		//	���̒����ɑ��݂���|���S���̐������J��Ԃ�
		HitRes = HitResDim.Dim;
		for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
		{
			//	�|���S���̍��W�͒n�ʃ|���S���̍��W
			Vertex[0].pos = HitRes->Position[0];
			Vertex[1].pos = HitRes->Position[1];
			Vertex[2].pos = HitRes->Position[2];

			//	������Ǝ����グ�ďd�Ȃ�Ȃ��悤�ɂ���
			SlideVec = VScale(HitRes->Normal, 0.5f);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			//	�|���S���̕s�����x��ݒ肷��
			Vertex[0].dif.a = 0;
			Vertex[1].dif.a = 0;
			Vertex[2].dif.a = 0;
			if (HitRes->Position[0].y > transform_->pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[0].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[0].y - transform_->pos.y) / PLAYER_SHADOW_HEIGHT)));

			if (HitRes->Position[1].y > transform_->pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[1].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[1].y - transform_->pos.y) / PLAYER_SHADOW_HEIGHT)));

			if (HitRes->Position[2].y > transform_->pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[2].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[2].y - transform_->pos.y) / PLAYER_SHADOW_HEIGHT)));

			//	�t�u�l�͒n�ʃ|���S���ƃv���C���[�̑��΍��W���犄��o��
			Vertex[0].u = (HitRes->Position[0].x - transform_->pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[0].v = (HitRes->Position[0].z - transform_->pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].u = (HitRes->Position[1].x - transform_->pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].v = (HitRes->Position[1].z - transform_->pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].u = (HitRes->Position[2].x - transform_->pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].v = (HitRes->Position[2].z - transform_->pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

			//	�e�|���S����`��
			DrawPolygon3D(Vertex, 1, imgShadow_, TRUE);
		}

		//	���o�����n�ʃ|���S�����̌�n��
		MV1CollResultPolyDimTerminate(HitResDim);
	}

	//	���C�e�B���O��L���ɂ���
	SetUseLighting(TRUE);

	//	�y�o�b�t�@�𖳌��ɂ���
	SetUseZBuffer3D(FALSE);


}

void PlayerBase::Rotate(void)
{

	stepRotTime_ -= scnMng_.GetDeltaTime();

	//	��]�̋��ʕ��
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);
}

void PlayerBase::Collision(void)
{
	//	���ݍ��W���N�_�Ɉړ�����W�����߂�
	movedPos_ = VAdd(transform_->pos, movePow_);

	//	�Փ�(�d��)
	CollisionGravity();

	//	�ړ�
	transform_->pos = movedPos_;

}

void PlayerBase::CollisionGravity(void)
{
	//	�W�����v�ʂ����Z
	movedPos_ = VAdd(movedPos_, jumpPow_);

	//	�d�͕���
	VECTOR dirGravity = AsoUtility::DIR_D;

	//	�d�͕����̔���
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	//	�d�͂̋���
	float gravityPow = DEFAULT_GRAVITY_POW;

	float checkPow = 10.0f;
	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));

	auto hit = colMng_.GetInstance().Line_IsCollision_Stage(gravHitPosUp_, gravHitPosDown_);

	if (hit.HitFlag> 0 && VDot(dirGravity, jumpPow_) > 0.9f)
	{
		//	�Փ˒n�_����A������Ɉړ�
		movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, gravityPow * 2.0f));

		//	�W�����v���Z�b�g
		jumpPow_ = AsoUtility::VECTOR_ZERO;
		stepJump_ = 0.0f;


		isJump_ = false;

		gravityPow_ = DEFAULT_GRAVITY_POW;

	}
}


float PlayerBase::Myself2OtherDistance(std::weak_ptr<Transform> toTran)
{
	VECTOR Dif = VSub(transform_->pos, toTran.lock()->pos);
	float DistanceXZ = sqrtf(powf(fabsf(Dif.x), 2) + powf(fabsf(Dif.z), 2));
	float Distance3D = sqrtf(powf(fabsf(DistanceXZ), 2) + powf(fabsf(Dif.y), 2));
	return Distance3D;
}

VECTOR PlayerBase::Myself2OtherDir(std::weak_ptr<Transform> toTran)
{
	VECTOR ret;

	VECTOR myPos = transform_->pos;
	VECTOR otherPos = toTran.lock()->pos;

	ret = AsoUtility::VNormalize(VSub(otherPos, myPos));
	return ret;
}

