#pragma once
#include <map>
#include "Common/Transform.h"
class ResourceManager;
class WarpStar;
class Planet;
class Raider;
class Survivor;

class Stage
{

public:

	//	ステージの切り替え間隔
	static constexpr float TIME_STAGE_CHANGE = 1.0f;

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

	void Init(void);
	void Update(void);
	void Draw(void);

	void SetObject(std::weak_ptr<Raider> raider, std::array<std::weak_ptr<Survivor>,3> survivor);

	//	ステージ変更
	void ChangeStage(NAME type);

	//	対象ステージを取得
	Planet* GetPlanet(NAME type);

private:

	//	シングルトン参照
	ResourceManager& resMng_;

	std::weak_ptr<Raider> raider_;
	std::array<std::weak_ptr<Survivor>,3> survivor_;

	//	ステージアクティブになっている惑星の情報
	NAME activeName_;
	Planet* activePlanet_;

	//	惑星
	std::map<NAME, Planet*> planets_;

	//	空のPlanet
	Planet* nullPlanet = nullptr;

	float step_;

	//	最初の惑星
	void MakeMainStage(void);

};
