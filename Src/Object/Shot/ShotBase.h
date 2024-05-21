#pragma once
#include "../ActorBase.h"

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
	bool GetAlive(void);
	
	void SetPos(VECTOR pos);

protected:


private:

};

