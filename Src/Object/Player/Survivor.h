#pragma once
#include "PlayerBase.h"

class Survivor : public PlayerBase
{

public:


	//	スピード
	static constexpr float SPEED_RUN = 15.0f;
	static constexpr float SPEED_FLY = 20.0f;
	static constexpr float SPEED_CRAWL = 3.0f;

	//	回転完了までの時間
	static constexpr float TIME_ROT = 0.3f;

	//	ジャンプ力
	static constexpr float POW_JUMP = 15.0f;

	//	ジャンプ受付時間
	static constexpr float TIME_JUMP_IN = 0.5f;

	static constexpr int FRAME_HEAD = 4;

	//	レイダーが殺した時に得るポイント 100で進化
	static constexpr int POINT_EVOLUTION = 50;

	static constexpr float REVIVE_TIMER = 1200;
	static constexpr float DEAD_TIMER = 1800;

	//	STATE::PLAY中のSTATE
	enum class STATE_INPLAY
	{
		NONE,
		IDLE,
		MOVE,
		JUMP,
		LAND,	//	着地
		FALL,	//	勝手に落下してる
		STUN,
		CRAWL,	//	死にかけで這っている
		DOWN,
	};

	//	アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		FAST_RUN,
		JUMP,
		WARP_PAUSE,
		FLY,
		FALLING,
		FLOAT,
		VICTORY,
		CRAWL_IDLE,
		CRAWL_MOVE,
		DOWN,
	};

	//	コンストラクタ(サバイバーのプレイヤー番号)
	Survivor(int survivorNum);

	//	デストラクタ
	~Survivor(void);

	void Init(void) override;
	void SetParam(void) override;
	void Update(void) override;
	void Draw(void) override;

	//	現在のSTATE::PLAY中ステートが入力したステートと同じか調べる
	bool IsStateInPlay(STATE_INPLAY state);

	void SetRaider(std::weak_ptr<Transform> tran);

	Survivor::STATE_INPLAY GetStatePlay(void);


	//	地上、空中関係ないSTATE::PLAY中ステートの切り替え
	void ChangeStateInPlay(STATE_INPLAY state);

	void OnCollision(std::weak_ptr<Collider> collider) override;

private:

	void InitAnimation(void) override;

	//	現在のSTATEに合わせて変わるアニメーション
	void ChangeStateAnimation(void) override;

	//	更新ステップ
	void UpdatePlay(void) override;

	//	地上、空中別の処理
	void UpdateLand(void);

	//	操作
	void ProcessMove(void) override;
	void ProcessJump(void) override;

	//	回転
	void SetGoalRotate(double rotRad) override;
	void CollisionGravity(void) override;

	//	移動量の計算
	void CalcGravityPow(void) override;

	//	着地モーション終了
	bool IsEndLanding(void) override;

	void CrawlDownUpdate(void);

	//	吹っ飛ばされ処理
	void BlowOff(void) override;

	//	スタン処理
	void Stun(void);

	//	HPが0になったときの処理
	virtual void KnockOuted(void) override;

	//	自分のプレイヤーNo.サバイバー内のみ
	int plNum_;

	//	STATE::PLAYの中のステート
	STATE_INPLAY statePlay_;

	//	飛翔判定
	bool isFly_;

	//	レイダーのTransform
	std::weak_ptr<Transform> raiderTran_;

	float reviveTimer_;
	float deadTimer_;
};

