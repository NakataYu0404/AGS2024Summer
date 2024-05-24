#pragma once
#include "../ActorBase.h"

class Victim : public ActorBase
{
public:

	//	レイダーが殺した時に得るポイント 100で進化
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

