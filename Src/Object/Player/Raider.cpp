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
#include "../Planet.h"
#include "../Shot/ShotBase.h"
#include "../Mob/Victim.h"
#include "Survivor.h"
#include "Raider.h"

Raider::Raider(void)
{
	SetParam();
}

Raider::~Raider(void)
{
}

void Raider::Init(void)
{
	transform_ = std::make_shared<Transform>();

	//	���f���̊�{�ݒ�
	transform_->SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::RAIDER));
	transform_->scl = AsoUtility::VECTOR_ONE;
	transform_->pos = { 0.0f, -30.0f, 0.0f };
	transform_->quaRot = Quaternion();
	transform_->quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_->Update();
	//	�A�j���[�V�����̐ݒ�
	InitAnimation();

	//	�J�v�Z���R���C�_
	capsule_ = std::make_shared<Capsule>(*transform_);
	capsule_->SetLocalPosTop({ 0.0f, 110.0f, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	capsule_->SetRadius(20.0f);

	//	�ۉe�摜
	imgShadow_ = resMng_.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;

	//	�������
	ChangeState(STATE::PLAY);

}

void Raider::SetParam(void)
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

	levelRaider_ = LEVEL_PL::LV1;
	exp_ = 0;

	for (int i = 0; i < SURVIVOR_NUM; i++)
	{
		R2SDistance_[i] = 0.0f;
	}
	isTarget_ = false;
	targetSurvivorNo_ = SURVIVOR_NUM;

	exeTarget_ = TARGET::NONE;

	ExecuteSur_ = nullptr;
	ExecuteVic_ = nullptr;

	exeCnt_ = EXECUTION_FLAME;

}

void Raider::Update(void)
{

	//	�X�V�X�e�b�v
	switch (state_)
	{
	case Raider::STATE::NONE:
		UpdateNone();
		break;
	case Raider::STATE::PLAY:
		UpdatePlay();
		break;
	}
	//	���f������X�V
	transform_->Update();
	UpdateShot();

	LockOn();


	//	�A�j���[�V�����Đ�
	animationController_->Update();

}

void Raider::Draw(void)
{

	//	���f���̕`��
	MV1DrawModel(transform_->modelId);

	DrawShot();

	//	�ۉe�`��
	DrawShadow();

	DrawFormatString(0, 0, 0x000000, "jumppowX:%fjumppowY:%fjumppowZ:%f", jumpPow_.x, jumpPow_.y, jumpPow_.z);
}

bool Raider::IsStateInPlay(STATE_INPLAY state)
{
	return statePlay_ == state;
}

void Raider::SetSurvivor(std::array<std::weak_ptr<Survivor>, SURVIVOR_NUM> surv)
{
	for (int i = 0; i < SURVIVOR_NUM; i++)
	{
		survivor_[i] = surv[i];
	}
	
}

void Raider::SetVictim(std::vector<std::weak_ptr<Victim>> tran)
{
	victim_ = tran;
}

bool Raider::IsWaitNow(void)
{
	if (waitFlame_ <= 0.0f)
	{
		return true;
	}

	return false;
}

void Raider::InitAnimation(void)
{

	std::string path = Application::PATH_MODEL + "Player/";
	animationController_ = std::make_shared<AnimationController>(transform_->modelId);
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

void Raider::ChangeStateAnimation(void)
{

	switch (statePlPos_)
	{
	case Raider::STATE_PLPOS::NONE:
		break;
	case Raider::STATE_PLPOS::LAND:

		switch (statePlay_)
		{
		case Raider::STATE_INPLAY::NONE:
			break;
		case Raider::STATE_INPLAY::IDLE:
			animationController_->Play((int)ANIM_TYPE::IDLE);
			break;
		case Raider::STATE_INPLAY::MOVE:
			animationController_->Play((int)ANIM_TYPE::RUN);
			break;
		case Raider::STATE_INPLAY::JUMP:
			break;
		case Raider::STATE_INPLAY::LAND:
			//	���n���[�V����
			animationController_->Play((int)ANIM_TYPE::JUMP, false, 29.0f, 45.0f, false, true);
			break;
		case Raider::STATE_INPLAY::FALL_MYSELF:
			animationController_->Play((int)ANIM_TYPE::FALLING);
			break;
		case Raider::STATE_INPLAY::FALL_NATURE:
			animationController_->Play((int)ANIM_TYPE::FALLING);
			break;
		case Raider::STATE_INPLAY::FLOAT:
			animationController_->Play((int)ANIM_TYPE::FLOAT);
			break;
		case Raider::STATE_INPLAY::STUN:
			break;
		default:
			break;
		}

		break;
	case Raider::STATE_PLPOS::AIR:

		switch (statePlay_)
		{
		case Raider::STATE_INPLAY::NONE:
			break;
		case Raider::STATE_INPLAY::IDLE:
			animationController_->Play((int)ANIM_TYPE::IDLE);
			break;
		case Raider::STATE_INPLAY::MOVE:
			animationController_->Play((int)ANIM_TYPE::FLY);
			break;
		case Raider::STATE_INPLAY::JUMP:
			break;
		case Raider::STATE_INPLAY::LAND:
			
			break;
		case Raider::STATE_INPLAY::FALL_MYSELF:
			animationController_->Play((int)ANIM_TYPE::FALLING);
			break;
		case Raider::STATE_INPLAY::FALL_NATURE:
			animationController_->Play((int)ANIM_TYPE::FALLING);
			break;
		case Raider::STATE_INPLAY::FLOAT:
			animationController_->Play((int)ANIM_TYPE::FLOAT);
			break;
		case Raider::STATE_INPLAY::STUN:
			break;
		default:
			break;
		}

		break;
	default:
		break;
	}
}

void Raider::UpdatePlay(void)
{
	switch (statePlPos_)
	{
	case Raider::STATE_PLPOS::LAND:
		UpdateLand();
		break;
	case Raider::STATE_PLPOS::AIR:
		UpdateAir();
		break;
	default:
		break;
	}

	Attack();
	PrepareExecution();
	ChangeLandAir();

	ChangeStateAnimation();

	//	�ړ������ɉ�������]
	Rotate();

	//	�d�͂ɂ��ړ���
	CalcGravityPow();

	//	�Փ˔���
	Collision();

	//	��]������
	transform_->quaRot = playerRotY_;

}

void Raider::UpdateShot(void)
{
	for (auto& i : shot_)
	{
		if (i == nullptr)
		{
			return;
		}

		i->Update();
	}
}

void Raider::DrawShot(void)
{
	for(auto& i:shot_)
	{
		if (i == nullptr)
		{
			//	shot_�z��̒��g��null�̉\��������̂�1�Ԗڂ����Ȃ̂ŁAnull�𔭌����������for�܂킳�Ȃ�
			return;
		}

		i->Draw();
	}
}


void Raider::UpdateLand(void)
{
	ProcessMove();
	if (levelRaider_ != LEVEL_PL::LV1)
	{
		//	��s����
		ProcessFly();
	}
	//	�W�����v����
	ProcessJump();

}

void Raider::UpdateAir(void)
{
	if (statePlay_ != STATE_INPLAY::ATTACK)
	{
		ProcessMoveFly();
	}
}

void Raider::ChangeLandAir(void)
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

void Raider::ChangeStateInPlay(STATE_INPLAY state)
{
	statePlay_ = state;
}

void Raider::ChangeIsFly(bool isFly)
{
	isFly_ = isFly;
}


void Raider::ProcessMove(void)
{

	auto& ins = InputManager::GetInstance();

	//	�ړ��ʂ��[��
	movePow_ = AsoUtility::VECTOR_ZERO;

	//	X����]���������A�d�͕����ɐ����ȃJ�����p�x(XZ����)���擾
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRotOutX();

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (!IsStateInPlay(STATE_INPLAY::FALL_MYSELF))
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

void Raider::ProcessJump(void)
{

	auto& ins = InputManager::GetInstance();
	bool isHit = ins.IsTrgDown(KEY_INPUT_SPACE);

	//	�W�����v����(�{�^����������Ă邯�ǃW�����v�������n���Ă�Ƃ��A�܂��̓W�����v���Œ��n���ĂȂ��Ƃ�)
 	if ((isHit && (isJump_ || IsEndLanding())) || (isJump_ && !IsEndLanding() && !IsStateInPlay(STATE_INPLAY::FALL_MYSELF)))
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

void Raider::ProcessFly(void)
{
	auto& ins = InputManager::GetInstance();
	bool isHit = ins.IsTrgDown(KEY_INPUT_SPACE);

  	if (isHit && (isJump_ && !IsEndLanding()))
	{
		ChangeIsFly(true);
		stepJump_ = TIME_JUMP_IN;
	}

	if (IsStateInPlay(STATE_INPLAY::FALL_MYSELF) && (ins.IsNew(KEY_INPUT_W) || ins.IsNew(KEY_INPUT_S) || ins.IsNew(KEY_INPUT_D) || ins.IsNew(KEY_INPUT_A)))
	{
		ChangeIsFly(true);
		ChangeStateInPlay(STATE_INPLAY::MOVE);
	}

}

void Raider::ProcessMoveFly(void)
{

	auto& ins = InputManager::GetInstance();

	//	�ړ��ʂ��[��
	movePow_ = AsoUtility::VECTOR_ZERO;

	//	X����]���������A�d�͕����ɐ����ȃJ�����p�x(XZ����)���擾
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRot();

	VECTOR dir = AsoUtility::VECTOR_ZERO;


	if (!IsStateInPlay(STATE_INPLAY::FALL_MYSELF))
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

	//	����ֈړ�������
	if (ins.IsNew(KEY_INPUT_SPACE))
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
			//	���ł���MOVE��
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

	//	���~������
	if (ins.IsDoubleClick(KEY_INPUT_SPACE))
	{
		//	�ړ��{�^����������������FLY�ɖ߂�~��
		ChangeIsFly(false);
		statePlay_ = STATE_INPLAY::FALL_MYSELF;
		gravityPow_ = 30.0f;
		SetGoalRotate(rotRad_);
	}
	else if (ins.IsTrgDown(KEY_INPUT_LCONTROL))
	{
		//	FLY�{�^���������Ȃ���FLY�ɖ߂�Ȃ��~��
		ChangeIsFly(false);
		statePlay_ = STATE_INPLAY::FALL_NATURE;
		SetGoalRotate(rotRad_);
	}

}

void Raider::Attack(void)
{
	if (!InputManager::GetInstance().IsTrgMouseLeft() && statePlay_ != STATE_INPLAY::ATTACK)
	{
		return;
	}

	if (!isTarget_)
	{
		MakeShot();
		statePlay_ = STATE_INPLAY::SHOT;
		return;
	}


	if (R2SDistance_[targetSurvivorNo_] < MAX_DISTANCE_ATTACKTARGET)
	{
		if (statePlay_ != STATE_INPLAY::ATTACK)
		{
			//	�ߐ�
			goalQuaRot_ = transform_->quaRot.LookRotation(R2SDir(targetSurvivorNo_));
			statePlay_ = STATE_INPLAY::ATTACK;
			ChangeIsFly(true);
			isJump_ = true;
		}
		else
		{
			//	���ɃA�^�b�N��Ԃɓ����Ă�
			transform_->pos = VAdd(transform_->pos, VScale(transform_->GetForward(), speed_ * 1.5f));
			goalQuaRot_ = transform_->quaRot.Slerp(transform_->quaRot, transform_->quaRot.LookRotation(R2SDir(targetSurvivorNo_)), (TIME_ROT - stepRotTime_) / TIME_ROT);
		}
	}
	else
	{
		MakeShot();
		statePlay_ = STATE_INPLAY::SHOT;
	}
}

void Raider::PrepareExecution(void)
{
	auto& ins = InputManager::GetInstance();


	if (ins.IsTrgDown(KEY_INPUT_E))
	{
		//	���Y�Ώۂ����߂邽�߂̏���
		float exeDistance = MAX_DISTANCE_EXECUTION;

		for (auto& s : survivor_)
		{
			if (s.lock() == nullptr)
			{
				continue;
			}

			float distance = Myself2OtherDistance(s.lock()->GetTransform());
			if (s.lock()->GetStatePlay() == Survivor::STATE_INPLAY::CRAWL && distance <= exeDistance)
			{
				exeTarget_ = TARGET::SURVIVOR;
				ExecuteSur_ = s.lock();
				exeDistance = distance;
			}
		}

		if (exeTarget_ != TARGET::SURVIVOR)
		{
			for (auto& v : victim_)
			{
				if (v.lock() == nullptr)
				{
					continue;
				}

				float distance = Myself2OtherDistance(v.lock()->GetTransform());
				if (distance <= exeDistance)
				{
					exeTarget_ = TARGET::VICTIM;
					ExecuteVic_ = v.lock();
					exeDistance = distance;
				}

			}
		}
	}

	//	���Y�{�^���������ŁA���Y
	if (!ins.IsNew(KEY_INPUT_E))
	{
		return;
	}

	switch (exeTarget_)
	{
	case Raider::TARGET::SURVIVOR:
		if (ExecuteSur_ != nullptr)
		{
			//�J�E���g�_�E��
			exeCnt_ -= 1.0f;
			if (exeCnt_ <= 0.0f)
			{
				exeCnt_ = EXECUTION_FLAME;
				Execution(ExecuteSur_);
			}
		}
		break;
	case Raider::TARGET::VICTIM:
		if (ExecuteVic_ != nullptr)
		{
			//�J�E���g�_�E��
			exeCnt_ -= 1.0f;
			if (exeCnt_ <= 0.0f)
			{
				exeCnt_ = EXECUTION_FLAME;
				Execution(ExecuteVic_);
			}
		}
		break;
	}
}

void Raider::Execution(std::shared_ptr<Survivor> target)
{
	target->SetState(PlayerBase::STATE::DEAD);
	exp_ += Survivor::POINT_EVOLUTION;
}

void Raider::Execution(std::shared_ptr<Victim> target)
{
	exp_ += Victim::POINT_EVOLUTION;
}

void Raider::MakeShot(void)
{
	//	�e
	for (auto& i : shot_)
	{
		//	i��null�Ȃ甲���� i��null�Ȃ͍̂ŏ�����
		if (i == nullptr)
		{
			break;
		}
		//	���񂾒e�ɑ��̐l�����܂���
		if (i->IsAlive() == false)
		{
			i->Init();
			ShotInit(i);	
			return;
		}
	}
	std::shared_ptr<ShotBase>shot = std::make_shared<ShotBase>();
	shot->Init();
	ShotInit(shot);
	shot_.push_back(shot);

}

void Raider::ShotInit(std::shared_ptr<ShotBase> shot)
{
	shot->SetPos(transform_->pos);
	shot->SetDir(ShotDir());
	shot->SetAlive(true);
}

void Raider::SetGoalRotate(double rotRad)
{
	VECTOR cameraRot;
	Quaternion axis;
	Quaternion axis2;

	switch (statePlPos_)
	{
	case Raider::STATE_PLPOS::LAND:

		cameraRot = SceneManager::GetInstance().GetCamera()->GetAngles();
		axis = Quaternion::AngleAxis((double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);

		break;

	case Raider::STATE_PLPOS::AIR:

		if (IsStateInPlay(STATE_INPLAY::FLOAT) || IsStateInPlay(STATE_INPLAY::FALL_MYSELF) || IsStateInPlay(STATE_INPLAY::FALL_NATURE))
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

void Raider::CollisionGravity(void)
{

	//	�W�����v�ʂ����Z
	movedPos_ = VAdd(movedPos_, jumpPow_);

	//	�d�͕���
	VECTOR dirGravity = AsoUtility::DIR_D;

	//	�d�͕����̔���
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	//	�d�͂̋���
	float gravityPow = gravityPow_;

	float checkPow = 20.0f;
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


void Raider::CalcGravityPow(void)
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

bool Raider::IsEndLanding(void)
{
	bool ret = true;

	//	�A�j���[�V�������W�����v�ł͂Ȃ�
	if (statePlPos_ == STATE_PLPOS::LAND && (!isJump_ && !IsStateInPlay(STATE_INPLAY::FALL_MYSELF)))
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

void Raider::LockOn(void)
{

	//	�t���O�Ǘ��̂��߂ɕK�v�ȏ�����
	float tmpKingDistance = MAX_DISTANCE_TARGET;	//	�Ƃ肠��������Ƃ����C�_�[���T�o�C�o�[�̈�ԒZ������
	targetSurvivorNo_ = SURVIVOR_NUM;	//	�^�[�Q�b�e�B���O����G�̔ԍ� 0~2�̏ꍇ�����ɓ���܂�
	isTarget_ = false;

	for (int i = 0; i < SURVIVOR_NUM; i++)
	{
		if (!CanTarget(i))
		{
			//	�^�[�Q�b�g�ł���Ȃ�A��
			continue;
		}
		//	���݈�ԒZ����Ƃ̔�r
		//	�����ɓ�����Ă��Ƃ́AMAX_DISTANCE_TARGET���͐�΂ɋ������Z��
		if (tmpKingDistance > R2SDistance_[i])
		{
			tmpKingDistance = R2SDistance_[i];
			targetSurvivorNo_ = i;
			isTarget_ = true;
		}
	}
}

bool Raider::CanTarget(int num)
{
	R2SDistance_[num] = Myself2OtherDistance(survivor_[num].lock()->GetTransform().lock());
	if(R2SDistance_[num] < MAX_DISTANCE_TARGET)
	{
		//���̊֐���FALSE�Ȃ�J�������ɓ����Ă���
		if(CheckCameraViewClip(survivor_[num].lock()->GetTransform().lock()->pos) == FALSE ||
			CheckCameraViewClip(survivor_[num].lock()->GetTransform().lock()->headPos) == FALSE)
		{
			return true;
		}
	}
	return false;
}

VECTOR Raider::ShotDir(void)
{
	//	�������瑊��Ɍ������x�N�g��������A�J������]�Ƃ��͊֌W�Ȃ�
	VECTOR ret;

	if (isTarget_)
	{
		VECTOR raiPos = transform_->pos;
		VECTOR suvPos = survivor_[targetSurvivorNo_].lock()->GetTransform().lock()->pos;

		ret = AsoUtility::VNormalize(VSub(suvPos, raiPos));
	}
	else
	{
		ret = SceneManager::GetInstance().GetCamera()->GetForward();
	}

	return ret;
}

VECTOR Raider::R2SDir(int num)
{
	VECTOR ret;

	VECTOR raiPos = transform_->pos;
	VECTOR suvPos = survivor_[num].lock()->GetTransform().lock()->pos;

	ret = AsoUtility::VNormalize(VSub(suvPos, raiPos));
	return ret;
}

//	TODO:�҂���Ԃ��ǂ������`�F�b�N���邽�߂̊֐��ɂȂ��Ă邯�ǁA�ړ��Ƃ��A�U���Ƃ������Ȃ̂Ɏg����������A�ėp���𑝂�����
//			���Ԃ��؂��܂ł��̏����������Ȃ��̂͂���܂�
void Raider::SetWaitFlame(float flame)
{
	if (flame < 0.0f)
	{
		return;
	}

	waitFlame_ = flame;
}

void Raider::WaitFlame(void)
{
	float delta = SceneManager::GetInstance().GetDeltaTime();
	waitFlame_ -= delta;
}

