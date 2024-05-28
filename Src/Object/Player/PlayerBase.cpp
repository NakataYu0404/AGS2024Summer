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
#include "../Planet.h"
#include "../Shot/ShotBase.h"
#include "PlayerBase.h"

PlayerBase::PlayerBase(void)
{
	SetParam();
}

PlayerBase::~PlayerBase(void)
{
}

void PlayerBase::AddCollider(Collider* collider)
{
	colliders_.push_back(collider);
}

void PlayerBase::ClearCollider(void)
{
	colliders_.clear();
}

const std::weak_ptr<Capsule> PlayerBase::GetCapsule(void) const
{
	return capsule_;
}

void PlayerBase::AddCapsule(Capsule* capsule)
{
}

void PlayerBase::ChangeState(STATE state)
{
	//	状態変更
	state_ = state;

	//	各状態遷移の初期処理
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
void PlayerBase::Rotate(void)
{

	stepRotTime_ -= scnMng_.GetDeltaTime();

	//	回転の球面補間
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);
}

void PlayerBase::Collision(void)
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

void PlayerBase::CollisionCapsule(void)
{

	//	トランスフォーム
	Transform trans = Transform(*transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(*capsule_, trans);

	//	コライダ
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
					//	移動
					trans.pos = movedPos_;
					trans.Update();
					continue;
				}

				break;

			}

		}

		//	あとしまつ
		MV1CollResultPolyDimTerminate(hits);

	}

}

float PlayerBase::Myself2OtherDistance(std::weak_ptr<Transform> toTran)
{
	VECTOR Dif = VSub(transform_->pos, toTran.lock()->pos);
	float DistanceXZ = sqrtf(powf(fabsf(Dif.x), 2) + powf(fabsf(Dif.z), 2));
	float Distance3D = sqrtf(powf(fabsf(DistanceXZ), 2) + powf(fabsf(Dif.y), 2));
	return Distance3D;
}