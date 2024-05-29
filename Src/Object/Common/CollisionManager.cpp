
#include"../../Utility/AsoUtility.h"
#include"Capsule.h"
#include"Collider.h"
#include "../Player/Raider.h"
#include "CollisionManager.h"

CollisionManager::CollisionManager(void)
{
}

CollisionManager::~CollisionManager(void)
{
}

void CollisionManager::Init(void)
{

	std::vector<Collider::Category> tags;

	//使用するタグを追加する
	tags.clear();
	tags.emplace_back(Collider::Category::SURVIVOR);
	tags.emplace_back(Collider::Category::STAGE);

	//当たり判定のためのタグ管理
	categoryMap_.emplace(Collider::Category::RAIDER, tags);


	//tags.clear();
	//tags.emplace_back(Collider::TAG::RAIDER);
	//categoryMap_.emplace(Collider::TAG::SURVIVOR, tags);

}

void CollisionManager::Update(void)
{

	for (auto actor : actors_)
	{
		//取得したコライダー情報
		//actor:typ,CAPSULE, tag, player
		auto actorCollider = actor.lock()->GetTransform().lock()->collider;
		auto actorType = actorCollider->type_;
		auto actorCategory = actorCollider->category_;

		//プレイヤー以外であればループを抜ける
		if (categoryMap_.count(actorCategory) == 0)
		{
			continue;
		}

		for (auto target : actors_)
		{
			//同じもの同士では判定しない
			if (actor.lock() == target.lock())
			{
				continue;
			}

			//それ以外のオブジェクト
			//target:typ,CAPSULE, tag, enemy stage
			auto targetCollider = target.lock()->GetTransform().lock()->collider;
			auto targetType = targetCollider->type_;
			auto targetCategory = targetCollider->category_;

			switch (actorType)
			{
			case Collider::TYPE::MODEL:
				break;
			case Collider::TYPE::CAPSULE:
				switch (targetType)
				{
				case Collider::TYPE::MODEL:		//カプセル対モデル
				{

					//auto info = Capsule2Model_Collider_PushBack(actor.lock()->GetCapsule(), target.lock()->GetTransform());

					//if (info.isHit)
					//{
					//	targetCollider->hitInfo_.isHit = true;
					//	targetCollider->hitInfo_.Normal = info.Normal;
					//	actor.lock()->OnCollision(targetCollider);
					//}
					//else
					//{
					//	targetCollider->hitInfo_.isHit = false;
					//	targetCollider->hitInfo_.Normal = { 0.0f, 0.0f, 0.0f };
					//}

				}
				break;
				case Collider::TYPE::CAPSULE:	//カプセル対カプセル
					if (Capsule2_Collider(
						actor.lock()->GetCapsule(),
						target.lock()->GetCapsule()
					))
					{
						actor.lock()->OnCollision(targetCollider);
						target.lock()->OnCollision(actorCollider);
					}
					break;
				}
				break;
			default:
				break;
			}

		}

	}

}

void CollisionManager::Draw(void)
{
	if (isAttack)
	{
		DrawString(10, 10, "当たった!!!", 0xff0000);
	}
}

void CollisionManager::Add(std::weak_ptr<ActorBase> collider)
{
	actors_.push_back(collider);
}

void CollisionManager::AddCollider(std::weak_ptr<Collider> collider)
{
	colliders_.push_back(collider);
}

void CollisionManager::ClearCollider(void)
{
	colliders_.clear();
}

Collider::Collision_Date CollisionManager::Capsule2Model_Collider_PushBack(const std::weak_ptr<Capsule> capsule, const std::weak_ptr<Transform> transform)
{

	Collider::Collision_Date answer;
	answer.isHit = false;

	//bool answer = false;

	answer.movedPos = capsule.lock()->GetCenter();


	// カプセルを移動させる	//TODO:Transform(transform)って何か理解する(Springのやつを見よう！)
	Transform trans = Transform(transform);
	trans.pos = answer.movedPos;
	trans.Update();
	Capsule cap = Capsule(capsule, trans);

	//// カプセルとの衝突判定
	//for (const auto c : colliders_)
	//{

	auto hits = MV1CollCheck_Capsule(
		transform.modelId, -1,
		cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());

	//DxLib::MV1_COLL_RESULT_POLY_DIM


	// 衝突した複数のポリゴンと衝突回避するまで、
	// プレイヤーの位置を移動させる
	for (int i = 0; i < hits.HitNum; i++)
	{

		auto hit = hits.Dim[i];
		//DxLib::MV1_COLL_RESULT_POLY

		// 地面と異なり、衝突回避位置が不明なため、何度か移動させる
		// この時、移動させる方向は、移動前座標に向いた方向であったり、
		// 衝突したポリゴンの法線方向だったりする
		for (int tryCnt = 0; tryCnt < 10; tryCnt++)
		{

			// 再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
			// 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
			int pHit = HitCheck_Capsule_Triangle(
				cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
				hit.Position[0], hit.Position[1], hit.Position[2]);
			if (pHit)
			{
				isAttack = true;
				Normal_ = hit.Normal;
				//// 法線の方向にちょっとだけ移動させる
				//auto movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));
				//answer.movedPos = VAdd(answer.movedPos, VScale(hit.Normal, 1.0f));


				// カプセルも一緒に移動させる
				//trans.pos = answer.movedPos;
				//trans.Update();


				answer.isHit = true;
				answer.Normal = hit.Normal;


				continue;
			}

			break;
		}
	}

	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);
	//}

	return answer;
}

bool CollisionManager::Capsule2_Collider(const std::weak_ptr<Capsule> a, const std::weak_ptr<Capsule> b)
{
	//弾が役割を終えていたら衝突判定を行わない
	//PlayerのGetAliveがfalseなら

	for (const auto i : actors_)
	{
		i.lock()->GetTransform().lock()->collider;
	}

	//プレイヤーと敵の衝突判定
	if (HitCheck_Capsule_Capsule(a.lock()->GetPosTop(), a.lock()->GetPosDown(), a.lock()->GetRadius(),
									b.lock()->GetPosTop(), b.lock()->GetPosDown(), b.lock()->GetRadius()))
	{
		//isAttack = true;
		return true;
	}
	else
	{
		isAttack = false;
		return false;
	}

}
