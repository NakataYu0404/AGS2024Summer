#pragma once
#include <Dxlib.h>
#include "EnemyBase.h"

class Enemy01 : public EnemyBase
{
public:
	Enemy01(void);
	//	デストラクタ
	~Enemy01(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	//	衝突判定に用いられるコライダ
	std::vector<Collider*> colliders_;
	Capsule* capsule_;

};

