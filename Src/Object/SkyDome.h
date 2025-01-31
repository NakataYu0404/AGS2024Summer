#pragma once
#include <map>
#include "Common/Transform.h"
#include "ActorBase.h"

class SkyDome : public ActorBase
{

public:

	static constexpr float SCALE = 100.0f;
	static constexpr VECTOR SCALES = { SCALE, SCALE, SCALE };

	//	状態
	enum class STATE
	{
		NONE,
		STAY,
		FOLLOW
	};

	//	コンストラクタ
	SkyDome(void);

	//	デストラクタ
	~SkyDome(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

private:

	//	状態遷移
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStateStay(void);
	void ChangeStateFollow(void);

	//	更新ステップ
	void UpdateNone(void);
	void UpdateStay(void);
	void UpdateFollow(void);

	//	状態
	STATE state_;

};
