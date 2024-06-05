
#include"../../Utility/AsoUtility.h"
#include"Capsule.h"
#include"Collider.h"
#include "../Player/Raider.h"
#include "CollisionManager.h"

CollisionManager* CollisionManager::instance_ = nullptr;

void CollisionManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new CollisionManager();
	}
	instance_->Init();
}

CollisionManager& CollisionManager::GetInstance(void)
{
	return *instance_;
}

void CollisionManager::Init(void)
{

	std::vector<Collider::Category> tags;

	//�g�p����^�O��ǉ�����
	tags.clear();
	tags.emplace_back(Collider::Category::SURVIVOR);
	tags.emplace_back(Collider::Category::STAGE);

	//�����蔻��̂��߂̃^�O�Ǘ�
	categoryMap_.emplace(Collider::Category::RAIDER, tags);
	categoryMap_.emplace(Collider::Category::SURVIVOR, tags);


	//tags.clear();
	//tags.emplace_back(Collider::TAG::RAIDER);
	//categoryMap_.emplace(Collider::TAG::SURVIVOR, tags);

}

void CollisionManager::Update(void)
{

	for (auto actor : actors_)
	{
		//�擾�����R���C�_�[���
		//actor:typ,CAPSULE, tag, player
		auto actorCollider = actor.lock()->GetTransform().lock()->collider_;
		auto actorType = actorCollider->type_;
		auto actorCategory = actorCollider->category_;

		//�v���C���[�ȊO�ł���΃��[�v�𔲂���
		if ((actorCategory != Collider::Category::RAIDER && actorCategory != Collider::Category::SURVIVOR) || categoryMap_.count(actorCategory) == 0)
		{
			continue;
		}

		for (auto target : actors_)
		{
			//�������̓��m�ł͔��肵�Ȃ�
			if (actor.lock() == target.lock())
			{
				continue;
			}

			//����ȊO�̃I�u�W�F�N�g
			//target:typ,CAPSULE, tag, enemy stage
			auto targetCollider = target.lock()->GetTransform().lock()->collider_;
			auto targetType = targetCollider->type_;
			auto targetCategory = targetCollider->category_;

			switch (actorType)
			{
			case Collider::TYPE::MODEL:
				break;
			case Collider::TYPE::CAPSULE:
				switch (targetType)
				{
				case Collider::TYPE::MODEL:		//�J�v�Z���΃��f��
				{

					auto info = Capsule2Model_Collider_PushBack(actor, target.lock()->GetTransform());

					if (info.isHit)
					{
						targetCollider->hitInfo_.isHit = true;
						targetCollider->hitInfo_.Normal = info.Normal;
						targetCollider->hitInfo_.movedPos = info.movedPos;
						actor.lock()->OnCollision(targetCollider);
					}
					else
					{
						targetCollider->hitInfo_.isHit = false;
						targetCollider->hitInfo_.Normal = { 0.0f, 0.0f, 0.0f };
					}

				}
				break;
				case Collider::TYPE::CAPSULE:	//�J�v�Z���΃J�v�Z��
					if (Capsule2_Collider(
						actor.lock()->GetCapsule().lock(),
						target.lock()->GetCapsule().lock()
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
		DrawString(10, 10, "��������!!!", 0xff0000);
	}
}

void CollisionManager::Destroy(void)
{
	delete instance_;
}

void CollisionManager::Add(std::shared_ptr<ActorBase> collider)
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

Collider::Collision_Date CollisionManager::Capsule2Model_Collider_PushBack(const std::weak_ptr<ActorBase> actor, const std::weak_ptr<Transform> transform)
{
	//	capsule���L�����N�^�[�ŁAtransform���X�e�[�W�̂��Ƃ��낤
	//	answer�������炭stage���̂Ȃ�

	Collider::Collision_Date answer;
	answer.isHit = false;

	answer.movedPos = actor.lock()->GetTransform().lock()->pos;


	// �J�v�Z�����ړ�������
	std::shared_ptr<Transform> trans = std::make_shared<Transform>();
	trans->pos = actor.lock()->GetTransform().lock()->pos;
	trans->Update();
	std::shared_ptr<Capsule> cap = std::make_shared<Capsule>(actor.lock()->GetCapsule(), trans);

	////// �J�v�Z���Ƃ̏Փ˔���
	//for (const auto c : colliders_)
	//{

		auto hits = MV1CollCheck_Capsule(
			transform.lock()->modelId, -1,
			cap->GetPosTop(), cap->GetPosDown(), cap->GetRadius());

		//DxLib::MV1_COLL_RESULT_POLY_DIM


		// �Փ˂��������̃|���S���ƏՓˉ������܂ŁA
		// �v���C���[�̈ʒu���ړ�������
		for (int i = 0; i < hits.HitNum; i++)
		{

			auto hit = hits.Dim[i];
			//DxLib::MV1_COLL_RESULT_POLY

			// �n�ʂƈقȂ�A�Փˉ���ʒu���s���Ȃ��߁A���x���ړ�������
			// ���̎��A�ړ�����������́A�ړ��O���W�Ɍ����������ł�������A
			// �Փ˂����|���S���̖@�������������肷��
			for (int tryCnt = 0; tryCnt < MAX_COLLISION_TRY; tryCnt++)
			{

				// �ēx�A���f���S�̂ƏՓˌ��o����ɂ́A���������߂���̂ŁA
				// �ŏ��̏Փ˔���Ō��o�����Փ˃|���S��1���ƏՓ˔�������
				int pHit = HitCheck_Capsule_Triangle(
					cap->GetPosTop(), cap->GetPosDown(), cap->GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]);
				if (pHit)
				{
					// �@���̕����ɂ�����Ƃ����ړ�������
					answer.movedPos = VAdd(answer.movedPos, VScale(hit.Normal, 2.0f));
					//�J�v�Z�����ꏏ�Ɉړ�������
					trans->pos = answer.movedPos;
					trans->Update();

					answer.isHit = true;
					answer.Normal = hit.Normal;


					continue;
				}

				break;
			}
		}

		// ���o�����n�ʃ|���S�����̌�n��
		MV1CollResultPolyDimTerminate(hits);
	//}

	return answer;
}

DxLib::MV1_COLL_RESULT_POLY CollisionManager::Line_IsCollision_Stage(const VECTOR LineTopPos, const VECTOR LineBotPos)
{
	MV1_COLL_RESULT_POLY a;

	if (this == nullptr)
	{
		a.HitFlag = false;
		return a;
	}

	//	�X�e�[�W���f���ƍs���̂ŁA�X�e�[�W��T���悤�ɂ���
	for (auto actor : actors_)
	{
		//�擾�����R���C�_�[���
		//actor:typ,CAPSULE, tag, player
		auto actorCollider = actor.lock()->GetTransform().lock()->collider_;
		auto actorCategory = actorCollider->category_;

		//�v���C���[�ȊO�ł���΃��[�v����蒼��
		if (actorCategory != Collider::Category::STAGE)
		{
			continue;
		}

		auto hit = MV1CollCheck_Line(actor.lock()->GetTransform().lock()->modelId, -1, LineTopPos, LineBotPos);

		return hit;
	}
}

bool CollisionManager::Capsule2_Collider(const std::weak_ptr<Capsule> a, const std::weak_ptr<Capsule> b)
{
	//�e���������I���Ă�����Փ˔�����s��Ȃ�
	//Player��GetAlive��false�Ȃ�

	for (const auto i : actors_)
	{
		i.lock()->GetTransform().lock()->collider_;
	}

	//�v���C���[�ƓG�̏Փ˔���
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
