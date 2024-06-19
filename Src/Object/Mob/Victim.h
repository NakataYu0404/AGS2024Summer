#pragma once
#include <memory>
#include "../ActorBase.h"

class AnimationController;


class Victim : public ActorBase
{
public:

	//	レイダーが殺した時に得るポイント 100で進化
	static constexpr int POINT_EVOLUTION = 25;

	//	状態
	enum class STATE
	{
		NONE,
		PLAY,
		DEAD,
		END
	};

	//	アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		DEAD,
	};


	Victim();
	~Victim();

	void Init() override;
	void Update() override;
	void Draw() override;
	void SetParam() override;

private:

	//	Anim初期化
	void InitAnimation(void);

	//	現在のSTATEに合わせて変わるアニメーション
	void ChangeStateAnimation(void);

	//	状態遷移
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStatePlay(void);

	//	更新ステップ
	void UpdateNone(void);
	void UpdatePlay(void);

	//	描画系
	void DrawShadow(void);

	//	生きてるか
	bool isAlive_;

	//	アニメーション
	std::shared_ptr<AnimationController> animationController_;

	//	状態管理
	STATE state_;

	//	丸影
	int imgShadow_;


};

