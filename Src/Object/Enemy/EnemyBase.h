#pragma once
#include <Dxlib.h>
#include "../ActorBase.h"
class AnimationController;
class Collider;
class Capsule;

class EnemyBase : public ActorBase
{
public:

	//	�R���X�g���N�^
	EnemyBase(void);

	//	�f�X�g���N�^
	~EnemyBase(void);

	void Init(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	virtual void SetParam(void) override;

	//	�Փ˔���ɗp������R���C�_����
	void AddCollider(Collider* collider);
	void ClearCollider(void);

	//	�Փ˗p�J�v�Z���̎擾
	const Capsule* GetCapsule(void) const;
};

