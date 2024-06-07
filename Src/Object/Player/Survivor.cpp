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
#include "Survivor.h"

Survivor::Survivor(int survivorNum)
{
	plNum_ = survivorNum;
	SetParam();
}

Survivor::~Survivor(void)
{
}

void Survivor::Init(void)
{
	transform_ = std::make_shared<Transform>();

	//	モデルの基本設定
	transform_->SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::MDL_SURVIVOR));
	transform_->scl = AsoUtility::VECTOR_ONE;
	transform_->pos = { 400.0f * (plNum_ + 1), -30.0f, 400.0f * (plNum_ + 1) };
	transform_->headPos = MV1GetFramePosition(transform_->modelId, FRAME_HEAD);
	transform_->quaRot = Quaternion();
	transform_->quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_->Update();
	//	アニメーションの設定
	InitAnimation();

	transform_->MakeCollider(Collider::Category::SURVIVOR, Collider::TYPE::CAPSULE);
	//	カプセルコライダ
	capsule_ = std::make_shared<Capsule>(transform_);
	capsule_->SetLocalPosTop({ 0.0f, 110.0f, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	capsule_->SetRadius(20.0f);


	//	丸影画像
	imgShadow_ = resMng_.Load(ResourceManager::SRC::IMG_PLAYERSHADOW).handleId_;

	//	初期状態
	ChangeState(STATE::PLAY);

}

void Survivor::SetParam(void)
{

	animationController_ = nullptr;
	state_ = STATE::NONE;
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

	//	衝突チェック
	gravHitPosDown_ = AsoUtility::VECTOR_ZERO;
	gravHitPosUp_ = AsoUtility::VECTOR_ZERO;

	imgShadow_ = -1;

	gravityPow_ = 10.0f;

	rotRad_ = 0.0f;

	hp_ = MAX_HP;
}

void Survivor::Update(void)
{

	//	更新ステップ
	switch (state_)
	{
	case Survivor::STATE::NONE:
		UpdateNone();
		break;
	case Survivor::STATE::PLAY:
		UpdatePlay();
		break;
	}


	//	モデル制御更新
	transform_->headPos = MV1GetFramePosition(transform_->modelId, FRAME_HEAD);

	transform_->Update();

	//	アニメーション再生
	animationController_->Update();

}

void Survivor::Draw(void)
{

	//	モデルの描画
	MV1DrawModel(transform_->modelId);

	//	丸影描画
	DrawShadow();

}

bool Survivor::IsStateInPlay(STATE_INPLAY state)
{
	return statePlay_ == state;
}

void Survivor::SetRaider(std::weak_ptr<Transform> tran)
{
	raiderTran_ = tran;
}

Survivor::STATE_INPLAY Survivor::GetStatePlay(void)
{
	return statePlay_;
}

void Survivor::SetState(PlayerBase::STATE state)
{
	state_ = state;
}

void Survivor::InitAnimation(void)
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
	animationController_->Add((int)ANIM_TYPE::CRAWL_IDLE, path + "CrawlMove.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::CRAWL_MOVE, path + "CrawlIdle.mv1", 60.0f);

	animationController_->Play((int)ANIM_TYPE::IDLE);

}

void Survivor::ChangeStateAnimation(void)
{

		switch (statePlay_)
		{
		case Survivor::STATE_INPLAY::NONE:
			break;
		case Survivor::STATE_INPLAY::IDLE:
			animationController_->Play((int)ANIM_TYPE::IDLE);
			break;
		case Survivor::STATE_INPLAY::MOVE:
			animationController_->Play((int)ANIM_TYPE::RUN);
			break;
		case Survivor::STATE_INPLAY::JUMP:
			break;
		case Survivor::STATE_INPLAY::LAND:
			//	着地モーション
			animationController_->Play((int)ANIM_TYPE::JUMP, false, 29.0f, 45.0f, false, true);
			break;
		case Survivor::STATE_INPLAY::STUN:
			break;
		case Survivor::STATE_INPLAY::CRAWL:
			animationController_->Play((int)ANIM_TYPE::CRAWL_MOVE);
			break;
		default:
			break;
		}

}

void Survivor::UpdatePlay(void)
{
	if (!IsStateInPlay(STATE_INPLAY::STUN))
	{
		UpdateLand();
	}

	ChangeStateAnimation();

	//	移動方向に応じた回転
	Rotate();

	//	重力による移動量
	CalcGravityPow();

	Stun();

	//	衝突判定
	Collision();

	//	回転させる
	transform_->quaRot = playerRotY_;

}

void Survivor::UpdateLand(void)
{
	ProcessMove();
	if (!IsStateInPlay(STATE_INPLAY::CRAWL))
	{
		//	ジャンプ処理
		ProcessJump();
	}
}

void Survivor::ChangeStateInPlay(STATE_INPLAY state)
{
	statePlay_ = state;
}

void Survivor::OnCollision(std::weak_ptr<Collider> collider)
{
	switch (collider.lock()->category_)
	{
	case Collider::Category::SURVIVOR:

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

void Survivor::ProcessMove(void)
{

	auto& ins = InputManager::GetInstance();

	//	移動量をゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	//	X軸回転を除いた、重力方向に垂直なカメラ角度(XZ平面)を取得
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRotOutX();

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	//	進みたい方向のキーが押されているか
	bool pushForward = false;
	bool pushBack = false;
	bool pushRight = false;
	bool pushLeft = false;
	switch (plNum_)
	{
	case 0:
		pushForward = ins.IsNew(KEY_INPUT_T);
		pushBack = ins.IsNew(KEY_INPUT_G);
		pushRight = ins.IsNew(KEY_INPUT_H);
		pushLeft = ins.IsNew(KEY_INPUT_F);
		break;
	case 1:
		pushForward = ins.IsNew(KEY_INPUT_I);
		pushBack = ins.IsNew(KEY_INPUT_K);
		pushRight = ins.IsNew(KEY_INPUT_L);
		pushLeft = ins.IsNew(KEY_INPUT_J);
		break;
	case 2:
		pushForward = ins.IsNew(KEY_INPUT_NUMPAD8);
		pushBack = ins.IsNew(KEY_INPUT_NUMPAD5);
		pushRight = ins.IsNew(KEY_INPUT_NUMPAD6);
		pushLeft = ins.IsNew(KEY_INPUT_NUMPAD4);
		break;
	}
	//	カメラ方向に前進したい
	if (pushForward)
	{
		rotRad_ = AsoUtility::Deg2RadD(0.0);
		dir = cameraRot.GetForward();
	}
	//	カメラ方向から後退したい
	if (pushBack)
	{
		rotRad_ = AsoUtility::Deg2RadD(180.0);
		dir = cameraRot.GetBack();
	}
	//	カメラ方向から右側へ移動したい
	if (pushRight)
	{
		rotRad_ = AsoUtility::Deg2RadD(90.0);
		dir = cameraRot.GetRight();
	}
	//	カメラ方向から左側へ移動したい
	if (pushLeft)
	{
		rotRad_ = AsoUtility::Deg2RadD(270.0);
		dir = cameraRot.GetLeft();
	}

	if (!AsoUtility::EqualsVZero(dir)/* && (isJump_ || IsEndLanding())*/) {

		//	移動処理
		if (!IsStateInPlay(STATE_INPLAY::CRAWL))
		{
			speed_ = SPEED_RUN;
		}
		else
		{
			speed_ = SPEED_CRAWL;
		}

		moveDir_ = dir;
		movePow_ = VScale(dir, speed_);

		//	回転処理
		SetGoalRotate(rotRad_);

		if ((!isJump_ && IsEndLanding()) && !IsStateInPlay(STATE_INPLAY::CRAWL))
		{
			ChangeStateInPlay(STATE_INPLAY::MOVE);
		}

	}
	else
	{
		if ((!isJump_ && IsEndLanding()) && !IsStateInPlay(STATE_INPLAY::CRAWL))
		{

			ChangeStateInPlay(STATE_INPLAY::IDLE);
		}
	}

}

void Survivor::ProcessJump(void)
{

	auto& ins = InputManager::GetInstance();
	bool isHit = ins.IsTrgDown(KEY_INPUT_SPACE);

	//	ジャンプ操作(ボタンが押されてるけどジャンプ中か着地してるとき、またはジャンプ中で着地してないとき)
	if ((isHit && (isJump_ || IsEndLanding())) || (isJump_ && !IsEndLanding()))
	{

		if (!isJump_)
		{
			//	制御無しジャンプ
			//	mAnimationController->Play((int)ANIM_TYPE::JUMP);
			//	ループしないジャンプ
			//	mAnimationController->Play((int)ANIM_TYPE::JUMP, false);
			//	切り取りアニメーション
			//	mAnimationController->Play((int)ANIM_TYPE::JUMP, false, 13.0f, 24.0f);
			//	無理やりアニメーション
			ChangeStateInPlay(STATE_INPLAY::JUMP);
			animationController_->Play((int)ANIM_TYPE::JUMP, true, 13.0f, 25.0f);
			animationController_->SetEndLoop(23.0f, 25.0f, 5.0f);

		}

		isJump_ = true;

		//	ジャンプの入力受付時間をヘラス
		stepJump_ += scnMng_.GetDeltaTime();
		if (stepJump_ < TIME_JUMP_IN)
		{
			jumpPow_ = VScale(AsoUtility::DIR_U, POW_JUMP);
		}

	}


}

void Survivor::SetGoalRotate(double rotRad)
{
	VECTOR cameraRot;
	Quaternion axis;
	Quaternion axis2;

		cameraRot = SceneManager::GetInstance().GetCamera()->GetAngles();
		axis = Quaternion::AngleAxis((double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);

	//	現在設定されている回転との角度差を取る
	double angleDiff = Quaternion::Angle(axis, goalQuaRot_);

	//	しきい値
	if (angleDiff > 0.1)
	{
		stepRotTime_ = TIME_ROT;
	}

	goalQuaRot_ = axis;

}

void Survivor::CollisionGravity(void)
{
	//	ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

	//	重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	//	重力方向の反対
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	//	重力の強さ
	float gravityPow = 1.0f;

	float checkPow = 20.0f;
	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));

	auto hit = colMng_.GetInstance().Line_IsCollision_Stage(gravHitPosUp_, gravHitPosDown_);

	if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.9f)
	{
		//	衝突地点から、少し上に移動
		movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, gravityPow * 2.0f));

		//	ジャンプリセット
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

void Survivor::CalcGravityPow(void)
{

	//	重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;
	
	//	重力の強さ
	gravityPow_ += 0.2f;

	//	重力
	VECTOR gravity = VScale(dirGravity, gravityPow_);
	jumpPow_ = VAdd(jumpPow_, gravity);

	//	最初は実装しない。地面と突き抜けることを確認する。
	//	内積
	float dot = VDot(dirGravity, jumpPow_);
	if (dot >= 0.0f)
	{
		//	重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
		jumpPow_ = gravity;
	}
}

bool Survivor::IsEndLanding(void)
{
	bool ret = true;

	if(!(statePlay_ == STATE_INPLAY::JUMP) && !(statePlay_ == STATE_INPLAY::FALL))
	{
		return ret;
	}
	
	//	アニメーションが終了しているか
	if (animationController_->IsEnd())
	{
		return ret;
	}

	return false;
}

void Survivor::BlowOff(void)
{
	ChangeStateInPlay(STATE_INPLAY::STUN);
	goalQuaRot_ = transform_->quaRot.LookRotation(Myself2OtherDir(raiderTran_));
}

void Survivor::Stun(void)
{
	if (!IsStateInPlay(STATE_INPLAY::STUN))
	{
		return;
	}
	movePow_ = AsoUtility::VECTOR_ZERO;
	jumpPow_ = VScale(blowOffVec_, blowOffPow_);
	stunTime_--;
	if (stunTime_ < 0)
	{
		ChangeStateInPlay(STATE_INPLAY::IDLE);
	}
}

void Survivor::KnockOuted(void)
{
	ChangeStateInPlay(STATE_INPLAY::CRAWL);
}
