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

	//	モデルの基本設定
	transform_->SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::RAIDER));
	transform_->scl = AsoUtility::VECTOR_ONE;
	transform_->pos = { 0.0f, -30.0f, 0.0f };
	transform_->quaRot = Quaternion();
	transform_->quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_->Update();
	//	アニメーションの設定
	InitAnimation();

	//	カプセルコライダ
	capsule_ = std::make_shared<Capsule>(*transform_);
	capsule_->SetLocalPosTop({ 0.0f, 110.0f, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	capsule_->SetRadius(20.0f);

	//	丸影画像
	imgShadow_ = resMng_.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;

	//	初期状態
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

	//	衝突チェック
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

	//	更新ステップ
	switch (state_)
	{
	case Raider::STATE::NONE:
		UpdateNone();
		break;
	case Raider::STATE::PLAY:
		UpdatePlay();
		break;
	}
	//	モデル制御更新
	transform_->Update();
	UpdateShot();

	LockOn();


	//	アニメーション再生
	animationController_->Update();

}

void Raider::Draw(void)
{

	//	モデルの描画
	MV1DrawModel(transform_->modelId);

	DrawShot();

	//	丸影描画
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
			//	着地モーション
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

	//	移動方向に応じた回転
	Rotate();

	//	重力による移動量
	CalcGravityPow();

	//	衝突判定
	Collision();

	//	回転させる
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
			//	shot_配列の中身がnullの可能性があるのは1番目だけなので、nullを発見したらもうforまわさない
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
		//	飛行処理
		ProcessFly();
	}
	//	ジャンプ処理
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

	//	移動量をゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	//	X軸回転を除いた、重力方向に垂直なカメラ角度(XZ平面)を取得
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRotOutX();

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (!IsStateInPlay(STATE_INPLAY::FALL_MYSELF))
	{
		//	カメラ方向に前進したい
		if (ins.IsNew(KEY_INPUT_W))
		{
			rotRad_ = AsoUtility::Deg2RadD(0.0);
			dir = cameraRot.GetForward();
		}

		//	カメラ方向から後退したい
		if (ins.IsNew(KEY_INPUT_S))
		{
			rotRad_ = AsoUtility::Deg2RadD(180.0);
			dir = cameraRot.GetBack();
		}

		//	カメラ方向から右側へ移動したい
		if (ins.IsNew(KEY_INPUT_D))
		{
			rotRad_ = AsoUtility::Deg2RadD(90.0);
			dir = cameraRot.GetRight();
		}

		//	カメラ方向から左側へ移動したい
		if (ins.IsNew(KEY_INPUT_A))
		{
			rotRad_ = AsoUtility::Deg2RadD(270.0);
			dir = cameraRot.GetLeft();
		}
	}

	if (!AsoUtility::EqualsVZero(dir)/* && (isJump_ || IsEndLanding())*/) {

		//	移動処理
		speed_ = SPEED_RUN;

		moveDir_ = dir;
		movePow_ = VScale(dir, speed_);

		//	回転処理
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

	//	ジャンプ操作(ボタンが押されてるけどジャンプ中か着地してるとき、またはジャンプ中で着地してないとき)
 	if ((isHit && (isJump_ || IsEndLanding())) || (isJump_ && !IsEndLanding() && !IsStateInPlay(STATE_INPLAY::FALL_MYSELF)))
	{
		if (!isJump_)
		{
			//	制御無しジャンプ
			//mAnimationController->Play((int)ANIM_TYPE::JUMP);
			//	ループしないジャンプ
			//mAnimationController->Play((int)ANIM_TYPE::JUMP, false);
			//	切り取りアニメーション
			//mAnimationController->Play((int)ANIM_TYPE::JUMP, false, 13.0f, 24.0f);
			
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

	//	移動量をゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	//	X軸回転を除いた、重力方向に垂直なカメラ角度(XZ平面)を取得
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRot();

	VECTOR dir = AsoUtility::VECTOR_ZERO;


	if (!IsStateInPlay(STATE_INPLAY::FALL_MYSELF))
	{
		//	カメラ方向に前進したい
		if (ins.IsNew(KEY_INPUT_W))
		{
			rotRad_ = AsoUtility::Deg2RadD(0.0);
			dir = cameraRot.GetForward();
		}

		//	カメラ方向から後退したい
		if (ins.IsNew(KEY_INPUT_S))
		{
			rotRad_ = AsoUtility::Deg2RadD(180.0);
			dir = cameraRot.GetBack();
		}

		//	カメラ方向から右側へ移動したい
		if (ins.IsNew(KEY_INPUT_D))
		{
			rotRad_ = AsoUtility::Deg2RadD(90.0);
			dir = cameraRot.GetRight();
		}

		//	カメラ方向から左側へ移動したい
		if (ins.IsNew(KEY_INPUT_A))
		{
			rotRad_ = AsoUtility::Deg2RadD(270.0);
			dir = cameraRot.GetLeft();
		}
	}

	//	上方へ移動したい
	if (ins.IsNew(KEY_INPUT_SPACE))
	{
		dir.y = 0.0f;
		dir = VAdd(dir,AsoUtility::AXIS_Y);
	}

	if (!AsoUtility::EqualsVZero(dir) && (isJump_ || IsEndLanding())) {

		//	移動処理
		speed_ = SPEED_FLY;

		moveDir_ = dir;
		movePow_ = VScale(dir, speed_);

		//	回転処理
		SetGoalRotate(rotRad_);

		if (statePlPos_ == STATE_PLPOS::AIR)
		{
			//	飛んでたらMOVEに
			ChangeStateInPlay(STATE_INPLAY::MOVE);
		}

		//上方向にのみいどうしてたら（上昇操作）
		if (dir.y == 1.0f && AsoUtility::EqualsVZero({ dir.x,0.0f,dir.z }))
		{
			ChangeStateInPlay(STATE_INPLAY::FLOAT);
		}
	}
	//移動してなかったら
	else if (AsoUtility::EqualsVZero(dir) && isFly_)
	{
		//	空中Idleに
		ChangeStateInPlay(STATE_INPLAY::IDLE);
	}

	//	下降したい
	if (ins.IsDoubleClick(KEY_INPUT_SPACE))
	{
		//	移動ボタンを押しただけでFLYに戻る降下
		ChangeIsFly(false);
		statePlay_ = STATE_INPLAY::FALL_MYSELF;
		gravityPow_ = 30.0f;
		SetGoalRotate(rotRad_);
	}
	else if (ins.IsTrgDown(KEY_INPUT_LCONTROL))
	{
		//	FLYボタンを押さないとFLYに戻らない降下
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
			//	近接
			goalQuaRot_ = transform_->quaRot.LookRotation(R2SDir(targetSurvivorNo_));
			statePlay_ = STATE_INPLAY::ATTACK;
			ChangeIsFly(true);
			isJump_ = true;
		}
		else
		{
			//	既にアタック状態に入ってる
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
		//	処刑対象を決めるための処理
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

	//	処刑ボタン長押しで、処刑
	if (!ins.IsNew(KEY_INPUT_E))
	{
		return;
	}

	switch (exeTarget_)
	{
	case Raider::TARGET::SURVIVOR:
		if (ExecuteSur_ != nullptr)
		{
			//カウントダウン
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
			//カウントダウン
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
	//	弾
	for (auto& i : shot_)
	{
		//	iがnullなら抜ける iがnullなのは最初だけ
		if (i == nullptr)
		{
			break;
		}
		//	死んだ弾に第二の人生歩ませる
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

		//もし空中で上昇しながらもXZ平面に移動してたら
		if (InputManager::GetInstance().IsNew(KEY_INPUT_SPACE) && IsStateInPlay(STATE_INPLAY::MOVE) && statePlPos_ == STATE_PLPOS::AIR)
		{
			//	x軸の角度を45度に
			cameraRot.x = AsoUtility::Deg2RadF(-45.0f);
		}

		axis2 = Quaternion::AngleAxis((double)cameraRot.x, AsoUtility::AXIS_X);
		axis = Quaternion::Mult(axis, axis2);
		break;

	}

	//	現在設定されている回転との角度差を取る
	double angleDiff = Quaternion::Angle(axis, goalQuaRot_);

	//	しきい値
	if (angleDiff > 0.1)
	{
		stepRotTime_ = TIME_ROT;
	}

	goalQuaRot_ = axis;

}

void Raider::CollisionGravity(void)
{

	//	ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

	//	重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	//	重力方向の反対
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	//	重力の強さ
	float gravityPow = gravityPow_;

	float checkPow = 20.0f;
	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));

	for (const auto c : colliders_)
	{
		
		//	地面との衝突
		auto hit = MV1CollCheck_Line(
			c->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		//	当たってるか、重力とジャンプのベクトル方向がほぼ一緒
		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.9f)
		{
			//	衝突地点から、少し上に移動
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 5.0f));

			//	ジャンプリセット
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
		//	飛んでるなら重力とジャンプ力を無効に
		jumpPow_ = AsoUtility::VECTOR_ZERO;
		gravityPow_ = Planet::DEFAULT_GRAVITY_POW;
		return;
	}

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

bool Raider::IsEndLanding(void)
{
	bool ret = true;

	//	アニメーションがジャンプではない
	if (statePlPos_ == STATE_PLPOS::LAND && (!isJump_ && !IsStateInPlay(STATE_INPLAY::FALL_MYSELF)))
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

void Raider::LockOn(void)
{

	//	フラグ管理のために必要な初期化
	float tmpKingDistance = MAX_DISTANCE_TARGET;	//	とりあえず入れとくレイダー→サバイバーの一番短い距離
	targetSurvivorNo_ = SURVIVOR_NUM;	//	ターゲッティングする敵の番号 0~2の場合処理に入ります
	isTarget_ = false;

	for (int i = 0; i < SURVIVOR_NUM; i++)
	{
		if (!CanTarget(i))
		{
			//	ターゲットできんなら帰る
			continue;
		}
		//	現在一番短いやつとの比較
		//	ここに入るってことは、MAX_DISTANCE_TARGETよりは絶対に距離が短い
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
		//この関数がFALSEならカメラ内に入っている
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
	//	自分から相手に向かうベクトルだから、カメラ回転とかは関係ない
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

//	TODO:待ち状態かどうかをチェックするための関数になってるけど、移動とか、攻撃とかいろんなのに使いたいから、汎用性を増させる
//			時間が切れるまでこの処理しかしないのはあんまり
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

