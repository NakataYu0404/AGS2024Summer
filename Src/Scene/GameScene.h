#pragma once
#include <array>
#include "SceneBase.h"
class Stage;
class SkyDome;
class Raider;
class Survivor;
class Victim;

class GameScene : public SceneBase
{
public:
	
	//	コンストラクタ
	GameScene(void);

	//	デストラクタ
	~GameScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	static constexpr int SURVIVOR_NUM = 3;

private:

	//	ステージ
	std::unique_ptr<Stage> stage_;

	//	スカイドーム
	std::unique_ptr<SkyDome> skyDome_;

	//	レイダー
	std::shared_ptr<Raider> raider_;

	//	サバイバー	1~3人と決まっているのでvectorにしない
	std::array<std::shared_ptr<Survivor>, SURVIVOR_NUM> survivors_;

	//	生贄モブ
	std::vector<std::shared_ptr<Victim>> victims_;
};
