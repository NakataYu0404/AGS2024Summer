#pragma once
#include <map>
#include <array>
#include "Common/Transform.h"
class ResourceManager;
class CollisionManager;
class WarpStar;
class Raider;
class Survivor;

class Stage : public ActorBase
{

public:

	//	ステージの切り替え間隔
	static constexpr float TIME_STAGE_CHANGE = 1.0f;
	static constexpr float DEFAULT_GRAVITY_POW = 1.0f;



	//	ステージ名
	enum class NAME
	{
		MAIN_PLANET,
		FALL_PLANET,
		FLAT_PLANET_BASE,
		FLAT_PLANET_ROT01,
	};

	//	コンストラクタ
	Stage(void);

	//	デストラクタ
	~Stage(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	void SetObject(std::weak_ptr<Raider> raider, std::array<std::weak_ptr<Survivor>, 3> survivor);


private:

	std::weak_ptr<Raider> raider_;
	std::array<std::weak_ptr<Survivor>,3> survivors_;

};
