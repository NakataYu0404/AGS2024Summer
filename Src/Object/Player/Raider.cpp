#include <string>
#include <DxLib.h>
#include "../../Application.h"
#include "../../Utility/AsoUtility.h"
#include "../../Manager/InputManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Camera.h"
#include "../Common/CollisionManager.h"
#include "../Common/AnimationController.h"
#include "../Common/Capsule.h"
#include "../Common/Sphere.h"
#include "../Common/Collider.h"
#include "../Common/CollisionManager.h"
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
		ResourceManager::SRC::MDL_RAIDER_LV1));
	transform_->scl = AsoUtility::VECTOR_ONE;
	transform_->pos = { 0.0f, 0.0f, 0.0f };
	transform_->headPos = MV1GetFramePosition(transform_->modelId, FRAME_HEAD_LV1);
	transform_->quaRot = Quaternion();
	transform_->quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_->Update();
	//	�A�j���[�V�����̐ݒ�
	InitAnimation();

	exeQube_ = std::make_shared<Transform>();
	exeQube_->SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::EXEQUBE));
	exeQube_->scl = {2.0f,2.0f,2.0f};
	exeQube_->pos = transform_->midPos;
	exeQube_->localPos = {0.0f,0.0f,0.0f};
	exeQube_->quaRot = transform_->quaRot;
	exeQube_->quaRotLocal = transform_->quaRotLocal;
	exeQube_->Update();

	transform_->MakeCollider(Collider::Category::RAIDER, Collider::TYPE::CAPSULE);

	//	�J�v�Z���R���C�_
	capsule_ = std::make_shared<Capsule>(transform_);
	capsule_->SetLocalPosTop({ 0.0f, 110.0f, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	capsule_->SetRadius(20.0f);
	
	//	�ۉe�摜
	imgShadow_ = resMng_.Load(ResourceManager::SRC::IMG_PLAYERSHADOW).handleId_;

	//	�������
	ChangeState(STATE::PLAY);

	attackType_ = ATTACK_TYPE::NONE;
	executionType_ = EXE_TYPE::NONE;
}

void Raider::SetParam(void)
{
	shotDestinationPos_ = AsoUtility::VECTOR_ZERO;
	cameraMidRayPos_ = AsoUtility::VECTOR_ZERO;

	animationController_ = nullptr;
	state_ = STATE::NONE;
	statePlPos_ = STATE_PLPOS::LAND;
	statePlay_ = STATE_INPLAY::IDLE;

	speed_ = SPEED_RUN_LV1;
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


	gravityPow_ = DEFAULT_GRAVITY_POW;

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
	chaseTime_ = CHASE_FLAME;
	attackCnt_ = ATTACK_FLAME;

	attackEndFlame_ = 15.0f;
	blowOffFlag_ = false;
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
	switch (levelRaider_)
	{
	case Raider::LEVEL_PL::LV1:
	transform_->headPos = MV1GetFramePosition(transform_->modelId, FRAME_HEAD_LV1);
		break;
	case Raider::LEVEL_PL::LV2:
	transform_->headPos = MV1GetFramePosition(transform_->modelId, FRAME_HEAD_LV2);
		break;
	case Raider::LEVEL_PL::LV3:
		break;
	}

	transform_->midPos = AsoUtility::VDiv(VAdd(transform_->pos, transform_->headPos), 2);
	transform_->Update();

	//	�A�j���[�V�����Đ�
	animationController_->Update();

}

void Raider::Draw(void)
{

	DrawLine3D(shotDestinationPos_, transform_->pos, 0x000000);

	//	���f���̕`��
	MV1DrawModel(transform_->modelId);

	if (IsStateInPlay(STATE_INPLAY::EXECUTION_LV1))
	{
		MV1DrawModel(exeQube_->modelId);
	}

	DrawShot();

	//	�ۉe�`��
	DrawShadow();

	DebugDraw();
}

void Raider::OnCollision(std::weak_ptr<Collider> collider)
{
	switch (collider.lock()->category_)
	{
	case Collider::Category::SURVIVOR:
		if (IsStateInPlay(STATE_INPLAY::ATTACK) && attackType_ == ATTACK_TYPE::CHASE && collider.lock()->modelId_ == survivor_[targetSurvivorNo_].lock()->GetTransform().lock()->modelId)
		{
			ChangeAttack(ATTACK_TYPE::HIT);
		}
		break;
	case Collider::Category::RAIDER:
		break;
	case Collider::Category::SHOT:
		break;
	case Collider::Category::STAGE:
		transform_->pos = collider.lock()->hitInfo_.movedPos;
		transform_->Update();
		break;
	default:
		break;
	}
}

void Raider::DebugDraw(void)
{
	DrawFormatString(0, 0, 0x000000, "jumppowX:%f,jumppowY:%f,jumppowZ:%f", jumpPow_.x, jumpPow_.y, jumpPow_.z);
	DrawFormatString(0, 30, 0x000000, "exp:%d", exp_);

}

bool Raider::IsStateInPlay(STATE_INPLAY state)
{
	return statePlay_ == state;
}

bool Raider::IsAttackType(ATTACK_TYPE type)
{
	return attackType_ == type;
}

bool Raider::IsExeType(EXE_TYPE type)
{
	return executionType_ == type;
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
	std::string path = Application::PATH_MODEL + "Player/Anim/";
	std::string path2 = Application::PATH_MODEL + "Player/Raider/Lv2Anim/";

	animationController_ = std::make_shared<AnimationController>(transform_->modelId);

	//int a = MV1LoadModel("Data/Model/");

	//animationController_->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::RUN, path + "Run.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::FAST_RUN, path + "FastRun.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::JUMP, path + "Jump.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FLY, path + "Flying.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FALLING, path + "FallIdle.mv1", 80.0f);
	animationController_->Add((int)ANIM_TYPE::FLOAT, path + "FloatIdle.mv1", 80.0f);
	animationController_->Add((int)ANIM_TYPE::VICTORY, path + "Victory.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::ATTACK_HIT, path + "manyPunch.mv1", 80.0f);
	animationController_->Add((int)ANIM_TYPE::ATTACK_END, path + "gurugurukick.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::EXECUTION_LV1, path + "fingerGun2.mv1", 120.0f);

	animationController_->Add((int)ANIM_TYPE::IDLE_LV2, path2 + "Idle2.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::RUN_LV2, path2 + "walk.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::FAST_RUN_LV2, path2 + "run.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::JUMP_LV2, path2 + "jump.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FLY_LV2, path2 + "idle1.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FALLING_LV2, path2 + "idle1.mv1", 80.0f);
	animationController_->Add((int)ANIM_TYPE::FLOAT_LV2, path2 + "idle1.mv1", 80.0f);
	animationController_->Add((int)ANIM_TYPE::VICTORY_LV2, path2 + "idle1.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::ATTACK_HIT_LV2, path2 + "atk1.mv1", 80.0f);
	animationController_->Add((int)ANIM_TYPE::ATTACK_END_LV2, path2 + "roar.mv1", 120.0f);
	animationController_->Add((int)ANIM_TYPE::EXECUTION_LV2, path2 + "atk2.mv1", 120.0f);

	animationController_->Play((int)ANIM_TYPE::IDLE);
	
}

void Raider::ChangeStateAnimation(void)
{
	switch (levelRaider_)
	{
	case Raider::LEVEL_PL::LV1:
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
			case Raider::STATE_INPLAY::EXECUTION_LV1:
				animationController_->Play((int)ANIM_TYPE::EXECUTION_LV1, false);
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
			case Raider::STATE_INPLAY::ATTACK:
				switch (attackType_)
				{
				case Raider::ATTACK_TYPE::CHASE:
					break;
				case Raider::ATTACK_TYPE::HIT:
					animationController_->Play((int)ANIM_TYPE::ATTACK_HIT);
					break;
				case Raider::ATTACK_TYPE::END:
					animationController_->Play((int)ANIM_TYPE::ATTACK_END, false);
					break;
				}
				break;
			case Raider::STATE_INPLAY::SHOT:
				break;
			default:
				break;
			}
			break;
		}

		break;
	case Raider::LEVEL_PL::LV2:
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
				animationController_->Play((int)ANIM_TYPE::IDLE_LV2);
				break;
			case Raider::STATE_INPLAY::MOVE:
				animationController_->Play((int)ANIM_TYPE::RUN_LV2);
				break;
			case Raider::STATE_INPLAY::JUMP:
				break;
			case Raider::STATE_INPLAY::LAND:
				//	���n���[�V����
				animationController_->Play((int)ANIM_TYPE::JUMP_LV2, false, 29.0f, 45.0f, false, true);
				break;
			case Raider::STATE_INPLAY::FALL_MYSELF:
				animationController_->Play((int)ANIM_TYPE::FALLING_LV2);
				break;
			case Raider::STATE_INPLAY::FALL_NATURE:
				animationController_->Play((int)ANIM_TYPE::FALLING_LV2);
				break;
			case Raider::STATE_INPLAY::FLOAT:
				animationController_->Play((int)ANIM_TYPE::FLOAT_LV2);
				break;
			case Raider::STATE_INPLAY::STUN:
				break;
			case Raider::STATE_INPLAY::EXECUTION_LV1:
				animationController_->Play((int)ANIM_TYPE::EXECUTION_LV2, false);
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
				animationController_->Play((int)ANIM_TYPE::IDLE_LV2);
				break;
			case Raider::STATE_INPLAY::MOVE:
				animationController_->Play((int)ANIM_TYPE::FLY_LV2);
				break;
			case Raider::STATE_INPLAY::JUMP:
				break;
			case Raider::STATE_INPLAY::LAND:

				break;
			case Raider::STATE_INPLAY::FALL_MYSELF:
				animationController_->Play((int)ANIM_TYPE::FALLING_LV2);
				break;
			case Raider::STATE_INPLAY::FALL_NATURE:
				animationController_->Play((int)ANIM_TYPE::FALLING_LV2);
				break;
			case Raider::STATE_INPLAY::FLOAT:
				animationController_->Play((int)ANIM_TYPE::FLOAT_LV2);
				break;
			case Raider::STATE_INPLAY::STUN:
				break;
			case Raider::STATE_INPLAY::ATTACK:
				switch (attackType_)
				{
				case Raider::ATTACK_TYPE::CHASE:
					break;
				case Raider::ATTACK_TYPE::HIT:
					animationController_->Play((int)ANIM_TYPE::ATTACK_HIT_LV2);
					break;
				case Raider::ATTACK_TYPE::END:
					animationController_->Play((int)ANIM_TYPE::ATTACK_END_LV2, false);
					break;
				}
				break;
			case Raider::STATE_INPLAY::SHOT:
				break;
			default:
				break;
			}
			break;
		}

		break;
	case Raider::LEVEL_PL::LV3:
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

	AttackStart();
	AttackHit();
	AttackEnd();

	ExeEvoUpdate();

	ChangeLandAir();

	ChangeStateAnimation();

	//	�ړ������ɉ�������]
	if (!IsStateInPlay(STATE_INPLAY::EXECUTION_LV1))
	{
		Rotate();
	}
	else
	{
		playerRotY_ = Quaternion::Quaternion();
		goalQuaRot_ = playerRotY_;
	}

	//	�d�͂ɂ��ړ���
	CalcGravityPow();

	//	�Փ˔���
	Collision();

	//	��]������
	transform_->quaRot = playerRotY_;

	ShotDestinationPosUpdate();

	UpdateShot();

	LockOn();
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
	if (levelRaider_ == LEVEL_PL::LV3)
	{
		//	��s����
		ProcessFly();
	}
	//	�W�����v����
	ProcessJump();

	PrepareExecution();
}

void Raider::UpdateAir(void)
{
	if (!IsStateInPlay(STATE_INPLAY::ATTACK))
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

	if (statePlay_ == STATE_INPLAY::ATTACK)
	{
		ChangeAttack(ATTACK_TYPE::CHASE);
	}
	else
	{
		ChangeAttack(ATTACK_TYPE::NONE);
	}
	
	if (statePlay_ != STATE_INPLAY::EXECUTION_LV1)
	{
		ChangeExecution(EXE_TYPE::NONE);
	}
}

void Raider::ChangeIsFly(bool isFly)
{
	isFly_ = isFly;
}

void Raider::ChangeAttack(ATTACK_TYPE type_)
{
	attackType_ = type_;
}

void Raider::ChangeExecution(EXE_TYPE type_)
{
	executionType_ = type_;
}


void Raider::ProcessMove(void)
{

	auto& ins = InputManager::GetInstance();

	//	�ړ��ʂ��[��
	movePow_ = AsoUtility::VECTOR_ZERO;

	//	X����]���������A�d�͕����ɐ����ȃJ�����p�x(XZ����)���擾
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRotOutX();

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (!IsStateInPlay(STATE_INPLAY::FALL_MYSELF) && !IsStateInPlay(STATE_INPLAY::EXECUTION_LV1))
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
		switch (levelRaider_)
		{
		case Raider::LEVEL_PL::LV1:
			speed_ = SPEED_RUN_LV1;
			break;
		case Raider::LEVEL_PL::LV2:
			speed_ = SPEED_RUN_LV2;
			break;
		case Raider::LEVEL_PL::LV3:
			break;
		}

		moveDir_ = dir;
		movePow_ = VScale(dir, speed_);

		//	��]����
		SetGoalRotate(rotRad_);

		if (!isJump_ && IsEndLanding() && !IsStateInPlay(STATE_INPLAY::EXECUTION_LV1))
		{
			ChangeStateInPlay(STATE_INPLAY::MOVE);
		}
		
	}
	else
	{
		if (!isJump_ && IsEndLanding() && !IsStateInPlay(STATE_INPLAY::EXECUTION_LV1))
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
			//	mAnimationController->Play((int)ANIM_TYPE::JUMP);
			//	���[�v���Ȃ��W�����v
			//	mAnimationController->Play((int)ANIM_TYPE::JUMP, false);
			//	�؂���A�j���[�V����
			//	mAnimationController->Play((int)ANIM_TYPE::JUMP, false, 13.0f, 24.0f);
			
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
			switch (levelRaider_)
			{
			case Raider::LEVEL_PL::LV1:
				jumpPow_ = VScale(AsoUtility::DIR_U, POW_JUMP_LV1);
				break;
			case Raider::LEVEL_PL::LV2:
				jumpPow_ = VScale(AsoUtility::DIR_U, POW_JUMP_LV2);
				break;
			case Raider::LEVEL_PL::LV3:
				break;
			}
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

		//	������ɂ݂̂��ǂ����Ă���i�㏸����j
		if (dir.y == 1.0f && AsoUtility::EqualsVZero({ dir.x,0.0f,dir.z }))
		{
			ChangeStateInPlay(STATE_INPLAY::FLOAT);
		}
	}
	//	�ړ����ĂȂ�������
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
		ChangeStateInPlay(STATE_INPLAY::FALL_MYSELF);
		gravityPow_ = 30.0f;
		SetGoalRotate(rotRad_);
	}
	else if (ins.IsTrgDown(KEY_INPUT_LCONTROL))
	{
		//	FLY�{�^���������Ȃ���FLY�ɖ߂�Ȃ��~��
		ChangeIsFly(false);
		ChangeStateInPlay(STATE_INPLAY::FALL_NATURE);
		SetGoalRotate(rotRad_);
	}

}

void Raider::AttackStart(void)
{
	if (!InputManager::GetInstance().IsTrgMouseLeft() && !IsAttackType(ATTACK_TYPE::CHASE))
	{
		return;
	}
	if (!isTarget_)
	{
		MakeShot();
		ChangeStateInPlay(STATE_INPLAY::SHOT);
		return;
	}


	if (R2SDistance_[targetSurvivorNo_] < MAX_DISTANCE_ATTACKTARGET)
	{
		if (!IsStateInPlay(STATE_INPLAY::ATTACK))
		{
			//	�ߐ�
			movePow_ = AsoUtility::VECTOR_ZERO;
			goalQuaRot_ = transform_->quaRot.LookRotation(Myself2OtherDir(survivor_[targetSurvivorNo_].lock()->GetTransform()));
			ChangeStateInPlay(STATE_INPLAY::ATTACK);
			ChangeIsFly(true);
			isJump_ = true;
			chaseTime_ = CHASE_FLAME;
		}
		else
		{
			//	���ɃA�^�b�N��Ԃɓ����Ă�
			transform_->pos = VAdd(transform_->pos, VScale(transform_->GetForward(), speed_ * 1.5f));
			goalQuaRot_ = transform_->quaRot.Slerp(transform_->quaRot, transform_->quaRot.LookRotation(Myself2OtherDir(survivor_[targetSurvivorNo_].lock()->GetTransform())), (TIME_ROT - stepRotTime_) / TIME_ROT);
			chaseTime_--;
			if (chaseTime_ <= 0)
			{
				chaseTime_ = CHASE_FLAME;
				if (levelRaider_ != LEVEL_PL::LV3)
				{
					ChangeIsFly(false);
				}
				ChangeStateInPlay(STATE_INPLAY::IDLE);
			}
		}
	}
	else
	{
		MakeShot();
		ChangeStateInPlay(STATE_INPLAY::SHOT);
	}
}

void Raider::AttackHit(void)
{
	if (!IsAttackType(ATTACK_TYPE::HIT))
	{
		return;
	}
	if (attackCnt_ < 0)
	{
		attackCnt_ = ATTACK_FLAME;
		ChangeAttack(ATTACK_TYPE::END);
		return;
	}
	survivor_[targetSurvivorNo_].lock()->SetBlowOff(AsoUtility::VECTOR_ZERO,0.0f,10.0f);
	attackCnt_--;
	return;
}

void Raider::AttackEnd(void)
{
	if (!IsAttackType(ATTACK_TYPE::END))
	{
		return;
	}
	else if (animationController_->GetPlayTime() <= attackEndFlame_)
	{
		survivor_[targetSurvivorNo_].lock()->SetBlowOff(AsoUtility::VECTOR_ZERO, 0.0f, 10.0f);
		return;
	}

	//	��񂾂��z���g�̐�����΂�����������悤�ɂ���������A�Ȃ񂩗ǂ��Ă��l������
	if (!blowOffFlag_)
	{
		survivor_[targetSurvivorNo_].lock()->SetBlowOff(survivor_[targetSurvivorNo_].lock()->GetTransform().lock()->GetUp(), ATTACK_POW, 120.0f);
		survivor_[targetSurvivorNo_].lock()->Damage(HALF_HP);
		blowOffFlag_ = true;
	}

	if (animationController_->IsEnd())
	{
		if (levelRaider_ != LEVEL_PL::LV3)
		{
			ChangeIsFly(false);
		}
		ChangeStateInPlay(STATE_INPLAY::IDLE);
		blowOffFlag_ = false;
	}
}

void Raider::PrepareExecution(void)
{
	auto& ins = InputManager::GetInstance();

	if (IsExeType(EXE_TYPE::EXEQUTED) || IsExeType(EXE_TYPE::EVOLUTION))
	{
		return;
	}

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
		exeCnt_ = EXECUTION_FLAME;
		exeTarget_ = TARGET::NONE;
		if (IsStateInPlay(STATE_INPLAY::EXECUTION_LV1) && IsExeType(EXE_TYPE::PREPARE))
		{
			SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FOLLOW);
			ChangeStateInPlay(STATE_INPLAY::IDLE);
			ChangeExecution(EXE_TYPE::NONE);
		}
		return;
	}
	else if (exeTarget_ != TARGET::NONE)
	{
		switch (levelRaider_)
		{
		case Raider::LEVEL_PL::LV1:
			SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::EXECUTION_LV1);
			break;
		case Raider::LEVEL_PL::LV2:
			SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::EXECUTION_LV2);
			break;
		case Raider::LEVEL_PL::LV3:
			break;
		}
		movePow_ = AsoUtility::VECTOR_ZERO;
		//	�{�^�������ĂāA�^�[�Q�b�g��NONE�ł�����������
		ChangeStateInPlay(STATE_INPLAY::EXECUTION_LV1);
		ChangeExecution(EXE_TYPE::PREPARE);
	}

	switch (exeTarget_)
	{
	case Raider::TARGET::SURVIVOR:
		if (ExecuteSur_ != nullptr)
		{
			//	�J�E���g�_�E��
			exeCnt_ -= 1.0f;
			if (exeCnt_ <= 0.0f)
			{
				exeCnt_ = EXECUTION_FLAME*3.0f;
				Execution(ExecuteSur_);
				exeTarget_ = TARGET::NONE;
			}
		}
		break;
	case Raider::TARGET::VICTIM:
		if (ExecuteVic_ != nullptr)
		{
			//	�J�E���g�_�E��
			exeCnt_ -= 1.0f;
			if (exeCnt_ <= 0.0f)
			{
				exeCnt_ = EXECUTION_FLAME*3.0f;
				Execution(ExecuteVic_);
				exeTarget_ = TARGET::NONE;
			}
		}
		break;
	}

}

void Raider::Execution(std::shared_ptr<Survivor> target)
{
	target->ChangeStateInPlay(Survivor::STATE_INPLAY::DOWN);
	exp_ += Survivor::POINT_EVOLUTION;
	ChangeExecution(EXE_TYPE::EXEQUTED);
}

void Raider::Execution(std::shared_ptr<Victim> target)
{
	exp_ += Victim::POINT_EVOLUTION;
	ChangeExecution(EXE_TYPE::EXEQUTED);
}

void Raider::Evolution(void)
{
	switch (levelRaider_)
	{
	case Raider::LEVEL_PL::LV1:
		exp_ = 0;
		levelRaider_ = Raider::LEVEL_PL::LV2;
		break;
	case Raider::LEVEL_PL::LV2:
		exp_ = 0;
		levelRaider_ = Raider::LEVEL_PL::LV3;
		break;
	case Raider::LEVEL_PL::LV3:
		exp_ = MAX_EVOLUTION_POINT;
		break;
	default:
		break;
	}
	
	ChangeExecution(EXE_TYPE::EVOLUTION);

}

void Raider::ExeEvoUpdate()
{
	if (!IsStateInPlay(STATE_INPLAY::EXECUTION_LV1))
	{
		return;
	}

	if (IsExeType(EXE_TYPE::EXEQUTED))
	{
		//TODO:��������@���Y��Ɛi���̃��[�r�[�A������
		if (exeCnt_ > 0)
		{
			exeCnt_--;
		}
		else
		{
			if (exp_ < MAX_EVOLUTION_POINT)
			{
				ChangeStateInPlay(STATE_INPLAY::IDLE);
				SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FOLLOW);
				return;
			}
			else
			{
				Evolution();
				exeCnt_ = EXECUTION_FLAME*8.0f;
			}
		}

	}
	if (IsExeType(EXE_TYPE::EVOLUTION))
	{
		if (exeCnt_ > 0)
		{
			exeCnt_--;
		}
		else
		{
			transform_->SetModel(resMng_.LoadModelDuplicate(
				ResourceManager::SRC::MDL_RAIDER_LV2));
			animationController_->ChangeModel(transform_->modelId);
			ChangeExecution(EXE_TYPE::NONE);
			ChangeStateInPlay(STATE_INPLAY::IDLE);
			SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FOLLOW);
		}

	}

	exeQube_->pos = transform_->midPos;
	exeQube_->pos.y += 200.0f;
	exeQube_->quaRot = transform_->quaRot;
	exeQube_->quaRotLocal = transform_->quaRotLocal;
	exeQube_->Update();
}

void Raider::MakeShot(void)
{
	//	�e�̍쐬
	for (auto& i : shot_)
	{
		//	i��null�Ȃ̂͒e���쐬���Ă��Ȃ����̂�
		if (i == nullptr)
		{
			break;
		}
		//	���񂾒e���ė��p(push_back�͏d��)
		if (i->IsAlive() == false)
		{
			i->Init();
			ShotInit(i);
			//	���dHit��h�����ߓ���������R���C�_�͏�����悤�ɂ��Ă���̂ŁA�ēx�o�^
			colMng_.Add(i);
			return;
		}
	}

	//	�e��V�K�쐬
	std::shared_ptr<ShotBase> shot = std::make_shared<ShotBase>();
	shot->Init();
	ShotInit(shot);
	shot_.push_back(shot);
	colMng_.Add(shot);
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

		//	�����󒆂ŏ㏸���Ȃ����XZ���ʂɈړ����Ă���
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
	float gravityPow = 1.0f;

	float checkPow = 10.0f;
	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));

	auto hit = colMng_.GetInstance().Line_IsCollision_Stage(gravHitPosUp_, gravHitPosDown_);

	if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.9f)
	{
		//	�Փ˒n�_����A������Ɉړ�
		movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, gravityPow * 2.0f));

		//	�W�����v���Z�b�g
		jumpPow_ = AsoUtility::VECTOR_ZERO;
		stepJump_ = 0.0f;

		if (isJump_)
		{
			statePlay_ = STATE_INPLAY::LAND;
		}

		isJump_ = false;

		gravityPow_ = DEFAULT_GRAVITY_POW;

	}
}

void Raider::CalcGravityPow(void)
{
	if (statePlPos_ == STATE_PLPOS::AIR)
	{
		//	���ł�Ȃ�d�͂ƃW�����v�͂𖳌���
		jumpPow_ = AsoUtility::VECTOR_ZERO;
		gravityPow_ = DEFAULT_GRAVITY_POW;
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
	if (IsStateInPlay(STATE_INPLAY::ATTACK))
	{
		return;
	}

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
		//	���̊֐���FALSE�Ȃ�J�������ɓ����Ă���
		if(CheckCameraViewClip(survivor_[num].lock()->GetTransform().lock()->pos) == FALSE ||
			CheckCameraViewClip(survivor_[num].lock()->GetTransform().lock()->headPos) == FALSE)
		{
			if (!survivor_[num].lock()->IsStateInPlay(Survivor::STATE_INPLAY::CRAWL) && !survivor_[num].lock()->IsStateInPlay(Survivor::STATE_INPLAY::DOWN))
			{
				return true;
			}
		}
	}
	return false;
}

VECTOR Raider::ShotDir(void)
{
	//	�������瑊��Ɍ������x�N�g��������A�J������]�Ƃ��͊֌W�Ȃ�
	VECTOR ret;
	VECTOR raiPos = transform_->pos;

	VECTOR s1 = survivor_[0].lock()->GetTransform().lock()->pos;
	VECTOR s2 = survivor_[1].lock()->GetTransform().lock()->pos;
	VECTOR s3 = survivor_[2].lock()->GetTransform().lock()->pos;

	if (isTarget_)
	{
		VECTOR suvPos = survivor_[targetSurvivorNo_].lock()->GetTransform().lock()->midPos;
		ret = AsoUtility::VNormalize(VSub(suvPos, raiPos));
	}
	else
	{
 		ret = AsoUtility::VNormalize(VSub(shotDestinationPos_, raiPos));
	}

	return ret;
}

//	TODO:�҂���Ԃ��ǂ������`�F�b�N���邽�߂̊֐��ɂȂ��Ă邯�ǁA�ړ��Ƃ��A�U���Ƃ������Ȃ̂Ɏg����������A�ėp���𑝂�����
//	���Ԃ��؂��܂ł��̏����������Ȃ��̂͂���܂�
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

void Raider::BlowOff(void)
{
	ChangeStateInPlay(STATE_INPLAY::STUN);
	
	movePow_ = VScale(blowOffVec_, blowOffPow_);
}

void Raider::KnockOuted(void)
{
	//	�����T�o�C�o�[�����V�[����
}

