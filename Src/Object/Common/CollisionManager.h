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

	// コンストラクタ
	CollisionManager(void);

	// デストラクタ
	virtual ~CollisionManager(void);

	void Init(void);
	void Update(void);
	void Draw(void);


	// 衝突判定に用いられるコライダ制御
	void AddCollider(std::weak_ptr<Collider> collider);
	void ClearCollider(void);

	void Add(std::weak_ptr<ActorBase> collider);

	//カプセルとモデルの衝突判定
	Collider::Collision_Date Capsule2Model_Collider_PushBack(const std::weak_ptr<Capsule> capsule, const std::weak_ptr<Transform> transform);

	//カプセルとカプセルの衝突判定
	bool Capsule2_Collider(const std::weak_ptr<Capsule> a, const std::weak_ptr<Capsule> b);

private:

	//当たったかの判定
	bool isAttack = false;

	//衝突したポリゴンの法線
	DxLib::VECTOR Normal_ = {};

	// 衝突判定に用いられるコライダ
	std::vector<std::weak_ptr<ActorBase>> actors_;
	std::vector<std::weak_ptr<Collider>> colliders_;

	//衝突した場合の関数
	std::function<void(void)> onCollision_;

	//衝突判定のための管理
	std::map<Collider::Category, std::vector<Collider::Category>> categoryMap_;

};
