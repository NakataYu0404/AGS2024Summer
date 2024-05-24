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
#include "Survivor.h"

Survivor::Survivor(int survivorNum)
{
	plNum_ = survivorNum;
	SetParam();
}

Survivor::~Survivor(void)
{
	delete capsule_;
	delete animationController_;
}

void Survivor::Init(void)
{
	transform_ = std::make_shared<Transform>();

	//	モデルの基本設定
	transform_->SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::PLAYER));
	transform_->scl = AsoUtility::VECTOR_ONE;
	transform_->pos = { 0.0f, -30.0f, 0.0f };
	transform_->headPos = MV1GetFramePosition(transform_->modelId, FRAME_HEAD);
	transform_->quaRot = Quaternion();
	transform_->quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_->Update();
	//	アニメーションの設定
	InitAnimation();

	//	カプセルコライダ
	capsule_ = new Capsule(*transform_);
	capsule_->SetLocalPosTop({ 0.0f, 110.0f, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	capsule_->SetRadius(20.0f);

	//	丸影画像
	imgShadow_ = resMng_.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;

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

	capsule_ = nullptr;

	gravityPow_ = Planet::DEFAULT_GRAVITY_POW;

	rotRad_ = 0.0f;
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

	DrawFormatString(0, 0, 0x000000, "jumppowX:%fjumppowY:%fjumppowZ:%f", jumpPow_.x, jumpPow_.y, jumpPow_.z);
}

void Survivor::AddCollider(Collider* collider)
{
	colliders_.push_back(collider);
}

void Survivor::ClearCollider(void)
{
	colliders_.clear();
}

const Capsule* Survivor::GetCapsule(void) const
{
	return capsule_;
}

void Survivor::AddCapsule(Capsule* capsule)
{
}

bool Survivor::IsStateInPlay(STATE_INPLAY state)
{
	return statePlay_ == state;
}

void Survivor::SetEnemy(std::weak_ptr<Transform> tran)
{
	enemyTran_ = tran;
}

void Survivor::InitAnimation(void)
{

	std::string path = Application::PATH_MODEL + "Player/";
	animationController_ = new AnimationController(transform_->modelId);
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

void Survivor::ChangeState(STATE state)
{

	//	状態変更
	state_ = state;

	//	各状態遷移の初期処理
	switch (state_)
	{
	case Survivor::STATE::NONE:
		ChangeStateNone();
		break;
	case Survivor::STATE::PLAY:
		ChangeStatePlay();
		break;
	}

}

void Survivor::ChangeStateNone(void)
{
}

void Survivor::ChangeStatePlay(void)
{
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
		default:
			break;
		}

}

void Survivor::UpdateNone(void)
{
}

void Survivor::UpdatePlay(void)
{
	UpdateLand();
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

void Survivor::DrawShadow(void)
{

	float PLAYER_SHADOW_HEIGHT = 300.0f;
	float PLAYER_SHADOW_SIZE = 30.0f;

	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3] = { VERTEX3D(), VERTEX3D(), VERTEX3D() };
	VECTOR SlideVec;
	int ModelHandle;

	//	ライティングを無効にする
	SetUseLighting(FALSE);

	//	Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	//	テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	//	影を落とすモデルの数だけ繰り返し
	for (const auto c : colliders_)
	{

		//	チェックするモデルは、jが0の時はステージモデル、1以上の場合はコリジョンモデル
		ModelHandle = c->modelId_;

		//	プレイヤーの直下に存在する地面のポリゴンを取得
		HitResDim = MV1CollCheck_Capsule(
			ModelHandle, -1,
			transform_->pos, VAdd(transform_->pos, { 0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f }), PLAYER_SHADOW_SIZE);

		//	頂点データで変化が無い部分をセット
		Vertex[0].dif = GetColorU8(255, 255, 255, 255);
		Vertex[0].spc = GetColorU8(0, 0, 0, 0);
		Vertex[0].su = 0.0f;
		Vertex[0].sv = 0.0f;
		Vertex[1] = Vertex[0];
		Vertex[2] = Vertex[0];

		//	球の直下に存在するポリゴンの数だけ繰り返し
		HitRes = HitResDim.Dim;
		for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
		{
			//	ポリゴンの座標は地面ポリゴンの座標
			Vertex[0].pos = HitRes->Position[0];
			Vertex[1].pos = HitRes->Position[1];
			Vertex[2].pos = HitRes->Position[2];

			//	ちょっと持ち上げて重ならないようにする
			SlideVec = VScale(HitRes->Normal, 0.5f);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			//	ポリゴンの不透明度を設定する
			Vertex[0].dif.a = 0;
			Vertex[1].dif.a = 0;
			Vertex[2].dif.a = 0;
			if (HitRes->Position[0].y > transform_->pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[0].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[0].y - transform_->pos.y) / PLAYER_SHADOW_HEIGHT)));

			if (HitRes->Position[1].y > transform_->pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[1].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[1].y - transform_->pos.y) / PLAYER_SHADOW_HEIGHT)));

			if (HitRes->Position[2].y > transform_->pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[2].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[2].y - transform_->pos.y) / PLAYER_SHADOW_HEIGHT)));

			//	ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
			Vertex[0].u = (HitRes->Position[0].x - transform_->pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[0].v = (HitRes->Position[0].z - transform_->pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].u = (HitRes->Position[1].x - transform_->pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].v = (HitRes->Position[1].z - transform_->pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].u = (HitRes->Position[2].x - transform_->pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].v = (HitRes->Position[2].z - transform_->pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

			//	影ポリゴンを描画
			DrawPolygon3D(Vertex, 1, imgShadow_, TRUE);
		}

		//	検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(HitResDim);
	}

	//	ライティングを有効にする
	SetUseLighting(TRUE);

	//	Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);

}


void Survivor::UpdateLand(void)
{
	ProcessMove();
	//	ジャンプ処理
	ProcessJump();

}

void Survivor::ChangeStateInPlay(STATE_INPLAY state)
{
	statePlay_ = state;
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

void Survivor::Rotate(void)
{

	stepRotTime_ -= scnMng_.GetDeltaTime();

	//	回転の球面補間
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);
}

void Survivor::Collision(void)
{

	//	現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_->pos, movePow_);

	//	衝突(カプセル)
	CollisionCapsule();

	//	衝突(重力)
	CollisionGravity();

	//	移動
	transform_->pos = movedPos_;

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

void Survivor::CollisionCapsule(void)
{

	//	カプセルを移動させる
	Transform trans = Transform(*transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(*capsule_, trans);
	//	カプセルとの衝突判定
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
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 2.0f));
					//	カプセルを移動させる
					trans.pos = movedPos_;
					trans.Update();
					continue;
				}

				break;

			}

		}

		//	検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);

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
