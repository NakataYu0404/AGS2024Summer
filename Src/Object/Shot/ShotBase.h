#pragma once
#include <memory>
#include "../ActorBase.h"

class CollisionManager;
class Sphere;

class ShotBase : public ActorBase
{
public:
	ShotBase(void);
	 ~ShotBase(void);

	virtual void Init(void) override;
	virtual void SetParam(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	VECTOR GetPos(void);
	bool IsAlive(void);
	
	void SetPos(VECTOR pos);
	void SetDir(VECTOR dir);
	void SetAlive(bool isAlive);

	void OnCollision(std::weak_ptr<Collider> collider) override;

protected:

	bool isAlive_;
	float speed_;
	VECTOR moveDir_;

private:

	//	シングルトン参照
	CollisionManager& colMng_;

};

