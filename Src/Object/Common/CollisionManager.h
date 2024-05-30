#pragma once
#include <vector>
#include <map>
#include <memory>
//#include <iostream>
#include <functional>
#include "Transform.h"
#include "../ActorBase.h"
#include "./Collider.h"

using namespace std;

class Transform;
class Collider;
class Capsule;
class ActorBase;
class Player;

class Shape
{

};

class CollisionManager
{
public:

	// �R���X�g���N�^
	CollisionManager(void);

	// �f�X�g���N�^
	virtual ~CollisionManager(void);

	void Init(void);
	void Update(void);
	void Draw(void);


	// �Փ˔���ɗp������R���C�_����
	void AddCollider(std::weak_ptr<Collider> collider);
	void ClearCollider(void);

	void Add(std::weak_ptr<ActorBase> collider);

	//�J�v�Z���ƃ��f���̏Փ˔���
	Collider::Collision_Date Capsule2Model_Collider_PushBack(const std::weak_ptr<Capsule> capsule, const std::weak_ptr<Transform> transform);

	//�J�v�Z���ƃJ�v�Z���̏Փ˔���
	bool Capsule2_Collider(const std::weak_ptr<Capsule> a, const std::weak_ptr<Capsule> b);

private:

	//�����������̔���
	bool isAttack = false;

	//�Փ˂����|���S���̖@��
	DxLib::VECTOR Normal_ = {};

	// �Փ˔���ɗp������R���C�_
	std::vector<std::weak_ptr<ActorBase>> actors_;
	std::vector<std::weak_ptr<Collider>> colliders_;

	//�Փ˂����ꍇ�̊֐�
	std::function<void(void)> onCollision_;

	//�Փ˔���̂��߂̊Ǘ�
	std::map<Collider::Category, std::vector<Collider::Category>> categoryMap_;

};
