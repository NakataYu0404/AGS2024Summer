#include <math.h>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/CollisionManager.h"
#include "Camera.h"

Camera::Camera(void) : colMng_(CollisionManager::GetInstance())
{
	angles_ = VECTOR();
	cameraUp_ = VECTOR();
	mode_ = MODE::NONE;
	pos_ = AsoUtility::VECTOR_ZERO;
	targetPos_ = AsoUtility::VECTOR_ZERO;
	followTransform_ = nullptr;
}

Camera::~Camera(void)
{
}

void Camera::Init(void)
{
	ChangeMode(MODE::FIXED_POINT);
	defaultMousePos_ = { Application::SCREEN_SIZE_X/2, Application::SCREEN_SIZE_Y/2 };
	midRayDestinationPos_ = AsoUtility::VECTOR_ZERO;
}

void Camera::Update(void)
{
	SetMousePoint(defaultMousePos_.x, defaultMousePos_.y);
}

void Camera::SetBeforeDraw(void)
{

	//	クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);
	
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	case Camera::MODE::FOLLOW_LV1:
		SetBeforeDrawFollowLv1();
		break;
	case Camera::MODE::FOLLOW_LV2:
		SetBeforeDrawFollowLv2();
		break;
	case Camera::MODE::EXECUTION_LV1:
		SetBeforeDrawExequtionLv1();
		break;
	case Camera::MODE::EXECUTION_LV2:
		SetBeforeDrawExequtionLv2();
		break;
	}

	//	カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		pos_, 
		targetPos_, 
		cameraUp_
	);

	midRayDestinationPos_ = VAdd(pos_, VScale(GetForward(), MIDPOS_STRETCH_POW));

	//	DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

}

void Camera::Draw(void)
{
}

void Camera::SetFollow(const Transform* follow)
{
	followTransform_ = follow;
}

VECTOR Camera::GetMidRayDestantionPos(void)
{
	return midRayDestinationPos_;
}

VECTOR Camera::GetPos(void) const
{
	return pos_;
}

VECTOR Camera::GetAngles(void) const
{
	return angles_;
}

VECTOR Camera::GetTargetPos(void) const
{
	return targetPos_;
}

Quaternion Camera::GetQuaRot(void) const
{
	return rot_;
}

Quaternion Camera::GetQuaRotOutX(void) const
{
	return rotOutX_;
}

VECTOR Camera::GetForward(void) const
{
	return VNorm(VSub(targetPos_, pos_));
}

void Camera::ChangeMode(MODE mode)
{

	//	カメラの初期設定
	SetDefault();

	//	カメラモードの変更
	mode_ = mode;

	//	変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		break;
	case Camera::MODE::FOLLOW_LV1:
		break;
	case Camera::MODE::EXECUTION_LV1:
		break;

	}

}

void Camera::SetDefault(void)
{

	//	カメラの初期設定
	pos_ = DEFAULT_CAMERA_POS;

	//	注視点
	targetPos_ = AsoUtility::VECTOR_ZERO;

	//	カメラの上方向
	cameraUp_ = AsoUtility::DIR_U;

	angles_.x = AsoUtility::Deg2RadF(30.0f);
	angles_.y = 0.0f;
	angles_.z = 0.0f;

	rot_ = Quaternion();

}

void Camera::SyncFollowLv1(void)
{

	//	同期先の位置
	VECTOR pos = followTransform_->pos;

	//	重力の方向制御に従う
	//	正面から設定されたY軸分、回転させる
	rotOutX_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	//	正面から設定されたX軸分、回転させる
	rot_ = rotOutX_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;

	//	注視点(通常重力でいうところのY値を追従対象と同じにする)
	localPos = rotOutX_.PosAxis(LOCAL_F2T_POS);
	targetPos_ = VAdd(pos, localPos);

	//	カメラ位置
	localPos = rot_.PosAxis(LOCAL_F2C_POS);

	VECTOR tmpPos = VAdd(pos, localPos);
	auto Hit = colMng_.Line_IsCollision_Stage(tmpPos, targetPos_);

	if (Hit.HitFlag)
	{
		pos_ = VAdd(Hit.HitPosition,VScale(GetForward(),50.0f));
		//	ここもうちょっと良い形を考えよう！球体のコライダ用意してもいいし・・・
	}
	else
	{
		pos_ = VAdd(pos, localPos);
	}

	//	カメラの上方向
	cameraUp_ = AsoUtility::DIR_U;

}

void Camera::SyncFollowLv2(void)
{

	//	同期先の位置
	VECTOR pos = followTransform_->pos;

	//	重力の方向制御に従う
	//	正面から設定されたY軸分、回転させる
	rotOutX_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	//	正面から設定されたX軸分、回転させる
	rot_ = rotOutX_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;

	//	注視点(通常重力でいうところのY値を追従対象と同じにする)
	localPos = rotOutX_.PosAxis(LOCAL_F2T_POS);
	targetPos_ = VAdd(pos, localPos);

	//	カメラ位置
	localPos = rot_.PosAxis(LOCAL_F2C_POS_LV2);

	VECTOR tmpPos = VAdd(pos, localPos);
	auto Hit = colMng_.Line_IsCollision_Stage(tmpPos, targetPos_);

	if (Hit.HitFlag)
	{
		pos_ = VAdd(Hit.HitPosition,VScale(GetForward(),50.0f));
		//	ここもうちょっと良い形を考えよう！球体のコライダ用意してもいいし・・・
	}
	else
	{
		pos_ = VAdd(pos, localPos);
	}

	//	カメラの上方向
	cameraUp_ = AsoUtility::DIR_U;

}

void Camera::ProcessRot(void)
{

	auto& ins = InputManager::GetInstance();
	float movePow = 5.0f;

	Vector2 mousePos = { defaultMousePos_.x - ins.GetMousePos().x, defaultMousePos_.y - ins.GetMousePos().y };
	
	//	カメラ回転
	if (mousePos.x < 0)
	{
		//	右回転
		angles_.y += AsoUtility::Deg2RadF((float)-mousePos.x / 10.0f);
	}
	if (mousePos.x > 0)
	{
		//	左回転
		angles_.y += AsoUtility::Deg2RadF((float)-mousePos.x/10.0f);
	}

	//	上回転
	if (mousePos.y < 0)
	{
		angles_.x += AsoUtility::Deg2RadF((float)-mousePos.y/10.0f);
		if (angles_.x > LIMIT_X_UP_RAD)
		{
			angles_.x = LIMIT_X_UP_RAD;
		}
	}

	//	下回転
	if (mousePos.y > 0)
	{
		angles_.x += AsoUtility::Deg2RadF((float)-mousePos.y/10.0f);
		if (angles_.x < -LIMIT_X_DW_RAD)
		{
			angles_.x = -LIMIT_X_DW_RAD;
		}
	}
}

void Camera::SetBeforeDrawFixedPoint(void)
{
	//	何もしない
}

void Camera::SetBeforeDrawFollowLv1(void)
{
	//	カメラ操作
	ProcessRot();

	//	追従対象との相対位置を同期
	SyncFollowLv1();
}

void Camera::SetBeforeDrawFollowLv2(void)
{
	//	カメラ操作
	ProcessRot();

	//	追従対象との相対位置を同期
	SyncFollowLv2();
}

void Camera::SetBeforeDrawSelfShot(void)
{
}

void Camera::SetBeforeDrawExequtionLv1(void)
{
	//	同期先の位置
	VECTOR pos = followTransform_->headPos;

	//	重力の方向制御に従う
	//	正面から設定されたY軸分、回転させる
	angles_ = Quaternion::ToEuler(followTransform_->quaRot.AngleAxis(AsoUtility::Deg2RadF(180.0f), AsoUtility::AXIS_Y));
	rotOutX_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);
	//	正面から設定されたX軸分、回転させる
	rot_ = rotOutX_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;

	//	注視点(通常重力でいうところのY値を追従対象と同じにする)
	localPos = rotOutX_.PosAxis(LOCAL_F2T_POS_EXE_LV1);
	targetPos_ = VAdd(pos, localPos);

	//	カメラ位置
	localPos = rot_.PosAxis(LOCAL_F2C_POS_EXE_LV1);
	pos_ = VAdd(pos, localPos);

	//	カメラの上方向
	cameraUp_ = AsoUtility::DIR_U;

}

void Camera::SetBeforeDrawExequtionLv2(void)
{
	//	同期先の位置
	VECTOR pos = followTransform_->headPos;

	//	重力の方向制御に従う
	//	正面から設定されたY軸分、回転させる
	angles_ = Quaternion::ToEuler(followTransform_->quaRot.AngleAxis(AsoUtility::Deg2RadF(180.0f),AsoUtility::AXIS_Y));
	rotOutX_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);
	//	正面から設定されたX軸分、回転させる
	rot_ = rotOutX_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;

	//	注視点(通常重力でいうところのY値を追従対象と同じにする)
	localPos = rotOutX_.PosAxis(LOCAL_F2T_POS_EXE_LV2);
	targetPos_ = VAdd(pos, localPos);

	//	カメラ位置
	localPos = rot_.PosAxis(LOCAL_F2C_POS_EXE_LV2);
	pos_ = VAdd(pos, localPos);

	//	カメラの上方向
	cameraUp_ = AsoUtility::DIR_U;

}
