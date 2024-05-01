#pragma once
#include <Dxlib.h>
#include "../ActorBase.h"
class AnimationController;
class Collider;
class Capsule;

class EnemyBase : public ActorBase
{
public:

	//	コンストラクタ
	EnemyBase(void);

	//	デストラクタ
	~EnemyBase(void);

	void Init(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	virtual void SetParam(void) override;

	//	衝突判定に用いられるコライダ制御
	void AddCollider(Collider* collider);
	void ClearCollider(void);

	//	衝突用カプセルの取得
	const Capsule* GetCapsule(void) const;
};

