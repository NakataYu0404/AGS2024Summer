#pragma once
#include <Dxlib.h>
#include "EnemyBase.h"

class Enemy01 : public EnemyBase
{
public:
	Enemy01(void);
	//	�f�X�g���N�^
	~Enemy01(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	//	�Փ˔���ɗp������R���C�_
	std::vector<Collider*> colliders_;
	Capsule* capsule_;

};

