#include <string>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Camera.h"
#include "Common/AnimationController.h"
#include "Common/Capsule.h"
#include "Common/Collider.h"
#include "Planet.h"
#include "Player.h"

Player::Player(void)
{

	animationController_ = nullptr;
	state_ = STATE::NONE;
	statePlPos_ = STATE_PLPOS::LAND;
	statePlay_ = STATE_INPLAY::IDLE;

	speed_ = 0.0f;
	moveDir_ = AsoUtility::VECTOR_ZERO;
	movePow_ = AsoUtility::VECTOR_ZERO;
	movedPos_ = AsoUtility::VECTOR_ZERO;

	playerRotY_ = Quaternion();
	goalQuaRot_ = Quaternion();
	stepRotTime_ = 0.0f;

	jumpPow_ = AsoUtility::VECTOR_ZERO;
	isJump_ = false;
	isFly_ = false;
	stepJump_ = 0.0f;

	//	�Փ˃`�F�b�N
	gravHitPosDown_ = AsoUtility::VECTOR_ZERO;
	gravHitPosUp_ = AsoUtility::VECTOR_ZERO;

	imgShadow_ = -1;

	capsule_ = nullptr;

	gravityPow_ = Planet::DEFAULT_GRAVITY_POW;

	rotRad_ = 0.0f;
}

Player::~Player(void)
{
	delete capsule_;
	delete animationController_;
}

void Player::Init(void)
{

	//	���f���̊�{�ݒ�
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::PLAYER));
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.pos = { 0.0f, -30.0f, 0.0f };
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_.Update();

	//	�A�j���[�V�����̐ݒ�
	InitAnimation();

	//	�J�v�Z���R���C�_
	capsule_ = new Capsule(transform_);
	capsule_->SetLocalPosTop({ 0.0f, 110.0f, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	capsule_->SetRadius(20.0f);

	//	�ۉe�摜
	imgShadow_ = resMng_.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;

	//	�������
	ChangeState(STATE::PLAY);

}

void Player::Update(void)
{

	//	�X�V�X�e�b�v
	switch (state_)
	{
	case Player::STATE::NONE:
		UpdateNone();
		break;
	case Player::STATE::PLAY:
		UpdatePlay();
		break;
	}

	//	���f������X�V
	transform_.Update();

	//	�A�j���[�V�����Đ�
	animationController_->Update();

}

void Player::Draw(void)
{

	//	���f���̕`��
	MV1DrawModel(transform_.modelId);

	//	�ۉe�`��
	DrawShadow();

	DrawFormatString(0, 0, 0x000000, "jumppowX:%fjumppowY:%fjumppowZ:%f", jumpPow_.x, jumpPow_.y, jumpPow_.z);
}

void Player::AddCollider(Collider* collider)
{
	colliders_.push_back(collider);
}

void Player::ClearCollider(void)
{
	colliders_.clear();
}

const Capsule* Player::GetCapsule(void) const
{
	return capsule_;
}

bool Player::IsStateInPlay(STATE_INPLAY state)
{
	return statePlay_ == state;
}

void Player::InitAnimation(void)
{

	std::string path = Application::PATH_MODEL + "Player/";
	animationController_ = new AnimationController(transform_.modelId);
	animationController_->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::RUN, path + "Run.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::FAST_RUN, path + "FastRun.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::JUMP, path + "Jump.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::WARP_PAUSE, path + "WarpPose.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FLY, path + "Flying.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FALLING, path + "FallIdle.mv1", 80.0f);
	animationController_->Add((int)ANIM_TYPE::FLOAT, path + "FloatIdle.mv1", 80.0f);
	animationController_->Add((int)ANIM_TYPE::VICTORY, path + "Victory.mv1", 60.0f);

	animationController_->Play((int)ANIM_TYPE::IDLE);

}

void Player::ChangeState(STATE state)
{

	//	��ԕύX
	state_ = state;

	//	�e��ԑJ�ڂ̏�������
	switch (state_)
	{
	case Player::STATE::NONE:
		ChangeStateNone();
		break;
	case Player::STATE::PLAY:
		ChangeStatePlay();
		break;
	}

}

void Player::ChangeStateNone(void)
{
}

void Player::ChangeStatePlay(void)
{
}

void Player::ChangeStateAnimation(void)
{

	switch (statePlPos_)
	{
	case Player::STATE_PLPOS::NONE:
		break;
	case Player::STATE_PLPOS::LAND:

		switch (statePlay_)
		{
		case Player::STATE_INPLAY::NONE:
			break;
		case Player::STATE_INPLAY::IDLE:
			animationController_->Play((int)ANIM_TYPE::IDLE);
			break;
		case Player::STATE_INPLAY::MOVE:
			animationController_->Play((int)ANIM_TYPE::RUN);
			break;
		case Player::STATE_INPLAY::JUMP:
			break;
		case Player::STATE_INPLAY::LAND:
			//	���n���[�V����
			animationController_->Play((int)ANIM_TYPE::JUMP, false, 29.0f, 45.0f, false, true);
			break;
		case Player::STATE_INPLAY::FALL:
			animationController_->Play((int)ANIM_TYPE::FALLING);
			break;
		case Player::STATE_INPLAY::FLOAT:
			animationController_->Play((int)ANIM_TYPE::FLOAT);
			break;
		case Player::STATE_INPLAY::ATTACK:
			break;
		case Player::STATE_INPLAY::SHOT:
			break;
		case Player::STATE_INPLAY::STUN:
			break;
		default:
			break;
		}

		break;
	case Player::STATE_PLPOS::AIR:

		switch (statePlay_)
		{
		case Player::STATE_INPLAY::NONE:
			break;
		case Player::STATE_INPLAY::IDLE:
			animationController_->Play((int)ANIM_TYPE::IDLE);
			break;
		case Player::STATE_INPLAY::MOVE:
			animationController_->Play((int)ANIM_TYPE::FLY);
			break;
		case Player::STATE_INPLAY::JUMP:
			break;
		case Player::STATE_INPLAY::LAND:
			
			break;
		case Player::STATE_INPLAY::FALL:
			animationController_->Play((int)ANIM_TYPE::FALLING);
			break;
		case Player::STATE_INPLAY::FLOAT:
			animationController_->Play((int)ANIM_TYPE::FLOAT);
			break;
		case Player::STATE_INPLAY::ATTACK:
			break;
		case Player::STATE_INPLAY::SHOT:
			break;
		case Player::STATE_INPLAY::STUN:
			break;
		default:
			break;
		}

		break;
	default:
		break;
	}
}

void Player::UpdateNone(void)
{
}

void Player::UpdatePlay(void)
{
	switch (statePlPos_)
	{
	case Player::STATE_PLPOS::LAND:
		UpdateLand();
		break;
	case Player::STATE_PLPOS::AIR:
		UpdateAir();
		break;
	default:
		break;
	}
	ChangeLandAir();
	ChangeStateAnimation();

	//	�ړ������ɉ�������]
	Rotate();

	//	�d�͂ɂ��ړ���
	CalcGravityPow();

	//	�Փ˔���
	Collision();

	//	��]������
	transform_.quaRot = playerRotY_;

}

void Player::DrawShadow(void)
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
			transform_.pos, VAdd(transform_.pos, { 0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f }), PLAYER_SHADOW_SIZE);

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
			if (HitRes->Position[0].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[0].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[0].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			if (HitRes->Position[1].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[1].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[1].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			if (HitRes->Position[2].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[2].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[2].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			//	�t�u�l�͒n�ʃ|���S���ƃv���C���[�̑��΍��W���犄��o��
			Vertex[0].u = (HitRes->Position[0].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[0].v = (HitRes->Position[0].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].u = (HitRes->Position[1].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].v = (HitRes->Position[1].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].u = (HitRes->Position[2].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].v = (HitRes->Position[2].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

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

void Player::UpdateLand(void)
{
	ProcessMove();
	//	�W�����v����
	ProcessJump();
	ProcessFly();

}

void Player::UpdateAir(void)
{
	ProcessMoveFly();

}

void Player::ChangeLandAir(void)
{
	if (isFly_)
	{
		statePlPos_ = STATE_PLPOS::AIR;
	}
	else
	{
		statePlPos_ = STATE_PLPOS::LAND;
	}
}

void Player::ChangeStateInPlay(STATE_INPLAY state)
{
	statePlay_ = state;
}

void Player::ProcessMove(void)
{

	auto& ins = InputManager::GetInstance();

	//	�ړ��ʂ��[��
	movePow_ = AsoUtility::VECTOR_ZERO;

	//	X����]���������A�d�͕����ɐ����ȃJ�����p�x(XZ����)���擾
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRotOutX();

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (!IsStateInPlay(STATE_INPLAY::FALL))
	{
		//	�J���������ɑO�i������
		if (ins.IsNew(KEY_INPUT_W))
		{
			rotRad_ = AsoUtility::Deg2RadD(0.0);
			dir = cameraRot.GetForward();
		}

		//	�J�������������ނ�����
		if (ins.IsNew(KEY_INPUT_S))
		{
			rotRad_ = AsoUtility::Deg2RadD(180.0);
			dir = cameraRot.GetBack();
		}

		//	�J������������E���ֈړ�������
		if (ins.IsNew(KEY_INPUT_D))
		{
			rotRad_ = AsoUtility::Deg2RadD(90.0);
			dir = cameraRot.GetRight();
		}

		//	�J�����������獶���ֈړ�������
		if (ins.IsNew(KEY_INPUT_A))
		{
			rotRad_ = AsoUtility::Deg2RadD(270.0);
			dir = cameraRot.GetLeft();
		}
	}

	if (!AsoUtility::EqualsVZero(dir)/* && (isJump_ || IsEndLanding())*/) {

		//	�ړ�����
		speed_ = SPEED_RUN;

		moveDir_ = dir;
		movePow_ = VScale(dir, speed_);

		//	��]����
		SetGoalRotate(rotRad_);

		if (!isJump_ && IsEndLanding())
		{
			ChangeStateInPlay(STATE_INPLAY::MOVE);
		}
		
	}
	else
	{
		if (!isJump_ && IsEndLanding())
		{

			ChangeStateInPlay(STATE_INPLAY::IDLE);
		}
	}

}

void Player::ProcessJump(void)
{

	auto& ins = InputManager::GetInstance();
	bool isHit = ins.IsTrgDown(KEY_INPUT_BACKSLASH);

	//	�W�����v����(�{�^����������Ă邯�ǃW�����v�������n���Ă�Ƃ��A�܂��̓W�����v���Œ��n���ĂȂ��Ƃ�)
 	if ((isHit && (isJump_ || IsEndLanding())) || (isJump_ && !IsEndLanding() && !IsStateInPlay(STATE_INPLAY::FALL)) )
	{

		if (!isJump_)
		{
			//	���䖳���W�����v
			//mAnimationController->Play((int)ANIM_TYPE::JUMP);
			//	���[�v���Ȃ��W�����v
			//mAnimationController->Play((int)ANIM_TYPE::JUMP, false);
			//	�؂���A�j���[�V����
			//mAnimationController->Play((int)ANIM_TYPE::JUMP, false, 13.0f, 24.0f);
			//	�������A�j���[�V����
				ChangeStateInPlay(STATE_INPLAY::JUMP);
				animationController_->Play((int)ANIM_TYPE::JUMP, true, 13.0f, 25.0f);
				animationController_->SetEndLoop(23.0f, 25.0f, 5.0f);

		}

		isJump_ = true;

		//	�W�����v�̓��͎�t���Ԃ��w���X
 		stepJump_ += scnMng_.GetDeltaTime();
		if (stepJump_ < TIME_JUMP_IN)
		{
 			jumpPow_ = VScale(AsoUtility::DIR_U, POW_JUMP);
		}

	}


}

void Player::ProcessFly(void)
{
	auto& ins = InputManager::GetInstance();
	bool isHit = ins.IsTrgDown(KEY_INPUT_SPACE);

  	if (isHit && (isJump_ && !IsEndLanding()))
	{
		isFly_ = true;
		stepJump_ = TIME_JUMP_IN;
	}
}

void Player::ProcessMoveFly(void)
{

	auto& ins = InputManager::GetInstance();

	//	�ړ��ʂ��[��
	movePow_ = AsoUtility::VECTOR_ZERO;

	//	X����]���������A�d�͕����ɐ����ȃJ�����p�x(XZ����)���擾
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRot();

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (!IsStateInPlay(STATE_INPLAY::FALL))
	{
		//	�J���������ɑO�i������
		if (ins.IsNew(KEY_INPUT_W))
		{
			rotRad_ = AsoUtility::Deg2RadD(0.0);
			dir = cameraRot.GetForward();
		}

		//	�J�������������ނ�����
		if (ins.IsNew(KEY_INPUT_S))
		{
			rotRad_ = AsoUtility::Deg2RadD(180.0);
			dir = cameraRot.GetBack();
		}

		//	�J������������E���ֈړ�������
		if (ins.IsNew(KEY_INPUT_D))
		{
			rotRad_ = AsoUtility::Deg2RadD(90.0);
			dir = cameraRot.GetRight();
		}

		//	�J�����������獶���ֈړ�������
		if (ins.IsNew(KEY_INPUT_A))
		{
			rotRad_ = AsoUtility::Deg2RadD(270.0);
			dir = cameraRot.GetLeft();
		}
	}

	//	���~������
	if (ins.IsDoubleClick(KEY_INPUT_SPACE))
	{
		isFly_ = false;
		statePlay_ = STATE_INPLAY::FALL;
		SetGoalRotate(rotRad_);
	}
	//	��ΓI�ȏ���ֈړ�������
	else if (ins.IsNew(KEY_INPUT_SPACE))
	{
		dir.y = 0.0f;
		dir = VAdd(dir,AsoUtility::AXIS_Y);
	}

	if (!AsoUtility::EqualsVZero(dir) && (isJump_ || IsEndLanding())) {

		//	�ړ�����
		speed_ = SPEED_FLY;

		moveDir_ = dir;
		movePow_ = VScale(dir, speed_);

		//	��]����
		SetGoalRotate(rotRad_);

		if (statePlPos_ == STATE_PLPOS::AIR)
		{
			//	�A�j���[�V����
			ChangeStateInPlay(STATE_INPLAY::MOVE);
		}

		//������ɂ݂̂��ǂ����Ă���i�㏸����j
		if (dir.y == 1.0f && AsoUtility::EqualsVZero({ dir.x,0.0f,dir.z }))
		{
			ChangeStateInPlay(STATE_INPLAY::FLOAT);
		}
	}
	//�ړ����ĂȂ�������
	else if (AsoUtility::EqualsVZero(dir) && isFly_)
	{
		//	��Idle��
		ChangeStateInPlay(STATE_INPLAY::IDLE);
	}

}

void Player::SetGoalRotate(double rotRad)
{
	VECTOR cameraRot;
	Quaternion axis;
	Quaternion axis2;

	switch (statePlPos_)
	{
	case Player::STATE_PLPOS::LAND:

		cameraRot = SceneManager::GetInstance().GetCamera()->GetAngles();
		axis = Quaternion::AngleAxis((double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);

		break;

	case Player::STATE_PLPOS::AIR:

		if (IsStateInPlay(STATE_INPLAY::FLOAT) || IsStateInPlay(STATE_INPLAY::FALL))
		{
			cameraRot = SceneManager::GetInstance().GetCamera()->GetAngles();
			axis = Quaternion::AngleAxis((double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);
			break;
		}

		cameraRot = SceneManager::GetInstance().GetCamera()->GetAngles();
		axis = Quaternion::AngleAxis((double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);

		cameraRot.x *= cosf(rotRad);

		//�����󒆂ŏ㏸���Ȃ����XZ���ʂɈړ����Ă���
		if (InputManager::GetInstance().IsNew(KEY_INPUT_SPACE) && IsStateInPlay(STATE_INPLAY::MOVE) && statePlPos_ == STATE_PLPOS::AIR)
		{
			//	x���̊p�x��45�x��
			cameraRot.x = AsoUtility::Deg2RadF(-45.0f);
		}

		axis2 = Quaternion::AngleAxis((double)cameraRot.x, AsoUtility::AXIS_X);
		axis = Quaternion::Mult(axis, axis2);
		break;

	}

	//	���ݐݒ肳��Ă����]�Ƃ̊p�x�������
	double angleDiff = Quaternion::Angle(axis, goalQuaRot_);

	//	�������l
	if (angleDiff > 0.1)
	{
		stepRotTime_ = TIME_ROT;
	}

	goalQuaRot_ = axis;

}

void Player::Rotate(void)
{

	stepRotTime_ -= scnMng_.GetDeltaTime();

	//	��]�̋��ʕ��
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);

}

void Player::Collision(void)
{

	//	���ݍ��W���N�_�Ɉړ�����W�����߂�
	movedPos_ = VAdd(transform_.pos, movePow_);

	//	�Փ�(�J�v�Z��)
	CollisionCapsule();

	//	�Փ�(�d��)
	CollisionGravity();

	//	�ړ�
	transform_.pos = movedPos_;

}

void Player::CollisionGravity(void)
{

	//	�W�����v�ʂ����Z
	movedPos_ = VAdd(movedPos_, jumpPow_);

	//	�d�͕���
	VECTOR dirGravity = AsoUtility::DIR_D;

	//	�d�͕����̔���
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	//	�d�͂̋���
	float gravityPow = gravityPow_;

	float checkPow = 10.0f;
	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));

	for (const auto c : colliders_)
	{
		
		//	�n�ʂƂ̏Փ�
		auto hit = MV1CollCheck_Line(
			c->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		//	�������Ă邩�A�d�͂ƃW�����v�̃x�N�g���������قڈꏏ
		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.9f)
		{

			//	�Փ˒n�_����A������Ɉړ�
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 5.0f));

			//	�W�����v���Z�b�g
			jumpPow_ = AsoUtility::VECTOR_ZERO;
			stepJump_ = 0.0f;

			if (isJump_)
			{
				statePlay_ = STATE_INPLAY::LAND;
			}

			isJump_ = false;

			gravityPow_ = Planet::DEFAULT_GRAVITY_POW;
		}

	}

}

void Player::CollisionCapsule(void)
{

	//	�J�v�Z�����ړ�������
	Transform trans = Transform(transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(*capsule_, trans);

	//	�J�v�Z���Ƃ̏Փ˔���
	for (const auto c : colliders_)
	{

		auto hits = MV1CollCheck_Capsule(
			c->modelId_, -1,
			cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());

		for (int i = 0; i < hits.HitNum; i++)
		{

			auto hit = hits.Dim[i];

			for (int tryCnt = 0; tryCnt < 10; tryCnt++)
			{

				int pHit = HitCheck_Capsule_Triangle(
					cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]);

				if (pHit)
				{
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));
					//	�J�v�Z�����ړ�������
					trans.pos = movedPos_;
					trans.Update();
					continue;
				}

				break;

			}

		}

		//	���o�����n�ʃ|���S�����̌�n��
		MV1CollResultPolyDimTerminate(hits);

	}

}

void Player::CalcGravityPow(void)
{
	if (statePlPos_ == STATE_PLPOS::AIR)
	{
		//	���ł�Ȃ�d�͂ƃW�����v�͂𖳌���
		jumpPow_ = AsoUtility::VECTOR_ZERO;
		gravityPow_ = Planet::DEFAULT_GRAVITY_POW;
		return;
	}

	//	�d�͕���
	VECTOR dirGravity = AsoUtility::DIR_D;

	//	�d�͂̋���
	gravityPow_ += 0.2f;

	//	�d��
	VECTOR gravity = VScale(dirGravity, gravityPow_);
	jumpPow_ = VAdd(jumpPow_, gravity);

	//	�ŏ��͎������Ȃ��B�n�ʂƓ˂������邱�Ƃ��m�F����B
	//	����
	float dot = VDot(dirGravity, jumpPow_);
	if (dot >= 0.0f)
	{
		//	�d�͕����Ɣ��Ε���(�}�C�i�X)�łȂ���΁A�W�����v�͂𖳂���
		jumpPow_ = gravity;
	}
}

bool Player::IsEndLanding(void)
{
	bool ret = true;

	//	�A�j���[�V�������W�����v�ł͂Ȃ�
	if (statePlPos_ == STATE_PLPOS::LAND && (!isJump_ && !IsStateInPlay(STATE_INPLAY::FALL)))
	{
		return ret;
	}

	//	�A�j���[�V�������I�����Ă��邩
	if (animationController_->IsEnd())
	{
		return ret;
	}

	return false;
}
