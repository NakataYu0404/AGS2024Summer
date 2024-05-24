#pragma once
#include "../ActorBase.h"

class Victim : public ActorBase
{
public:

	//	���C�_�[���E�������ɓ���|�C���g 100�Ői��
	static constexpr int POINT_EVOLUTION = 25;

	Victim();
	~Victim();

	void Init() override;
	void Update() override;
	void Draw() override;
	void SetParam() override;

private:

	bool isAlive_;
	
	

};

