#pragma once
#include "PlayerBase.h"
class ShotBase;

class Raider : public PlayerBase
{

public:

	//	スピード
	static constexpr float SPEED_RUN = 15.0f;
	static constexpr float SPEED_FLY = 20.0f;

	//	回転完了までの時間
	static constexpr float TIME_ROT = 0.3f;

	//	ジャンプ力
	static constexpr float POW_JUMP = 15.0f;

	//	ジャンプ受付時間
	static constexpr float TIME_JUMP_IN = 0.5f;

	//	サバイバーの数
	//	GameSceneのやつを使えた方がいいけど、ヘッダーでインクルードすることになって良くない
	static constexpr int SURVIVOR_NUM = 3;

	
	//	空中にいたいか地上にいたいかで処理を変えたい(不可抗力で空中にいてしまってる場合は(落下とか)LANDで、自分から浮いてるときはAIR)
	enum class STATE_PLPOS
	{
		NONE,
		LAND,
		AIR,
	};

	//	STATE::PLAY中のSTATE
	enum class STATE_INPLAY
	{
		NONE,
		IDLE,
		MOVE,
		JUMP,
		LAND,	//	着地
		FALL_MYSELF,	//	自分で降下してる
		FALL_NATURE,	//	勝手に落下してる
		FLOAT,	//	自分で上昇してる
		STUN,
		ATTACK,
		SHOT,
	};

	enum class LEVEL_PL
	{
		LV1,
		LV2,
		LV3,
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
	};

	enum class TARGET
	{
		NONE,
		SURVIVOR,
		VICTIM,
	};

	//	コンストラクタ
	Raider(void);

	//	デストラクタ
	~Raider(void);

	void Init(void) override;
	void SetParam(void) override;
	void Update(void) override;
	void Draw(void) override;

	//	現在のSTATE::PLAY中ステートが入力したステートと同じか調べる
	bool IsStateInPlay(STATE_INPLAY state);

	void SetSurvivor(std::array<std::weak_ptr<Transform>, SURVIVOR_NUM> tran);

	bool IsWaitNow(void);
		
private:

	//	ターゲット範囲の最大距離
	static constexpr float MAX_DISTANCE_TARGET = 3000.0f;
	static constexpr float MAX_DISTANCE_ATTACKTARGET = MAX_DISTANCE_TARGET / 3.0f;

	static constexpr float MAX_DISTANCE_EXECUTION = 300;

	//	Anim初期化
	void InitAnimation(void) override;

	//	現在のSTATEに合わせて変わるアニメーション
	void ChangeStateAnimation(void) override;

	//	更新ステップ
	void UpdatePlay(void) override;

	//	弾の更新
	void UpdateShot(void);

	//	描画系
	void DrawShot(void);

	//	地上、空中別の処理
	void UpdateLand(void);
	void UpdateAir(void);

	//	地上、空中の切り替え
	void ChangeLandAir(void);

	//	地上、空中関係ないSTATE::PLAY中ステートの切り替え
	void ChangeStateInPlay(STATE_INPLAY state);

	//	飛んでるかフラグをここで設定する(現在レベルによって変えるかどうか決められる)
	void ChangeIsFly(bool isFly);

	//	移動
	void ProcessMove(void) override;
	void ProcessJump(void) override;
	void ProcessFly(void);
	void ProcessMoveFly(void);

	//	攻撃
	void Attack(void);
	void MakeShot(void);

	//	弾初期化
	void ShotInit(std::shared_ptr<ShotBase> shot);

	//	処刑準備(ボタン長押し)
	void PrepareExecution(Transform target);

	//	処刑
	void Execution(Transform target);

	//	回転
	void SetGoalRotate(double rotRad) override;

	//	衝突判定
	void CollisionGravity(void) override;

	//	移動量の計算
	void CalcGravityPow(void) override;

	//	着地モーション終了
	bool IsEndLanding(void) override;

	//	ロックオン
	void LockOn(void);

	//	trueでターゲッティング
	bool CanTarget(int num);
	VECTOR ShotDir(void);

	//	レイダー→サバイバーの向き(正規化済み)
	VECTOR R2SDir(int num);

	void SetWaitFlame(float flame = -1);
	void WaitFlame(void);

	//	現在の処刑可能対象(範囲内でサバイバー優先)
	TARGET exeTarget_;

	//	プレイヤーがいるのが地上か空中かを可視化
	STATE_PLPOS statePlPos_;

	//	STATE::PLAYの中のステート
	STATE_INPLAY statePlay_;

	//	レイダーのレベル
	LEVEL_PL levelRaider_;

	//	移動スピード
	float speed_;

	//	飛翔判定
	bool isFly_;

	//	サバイバー、生贄のTransform
	std::array<std::weak_ptr<Transform>, SURVIVOR_NUM> survivorTran_;
	std::vector<std::weak_ptr<Transform>> victimTran_;

	//	レイダーからサバイバーへの距離
	std::array<float, SURVIVOR_NUM> R2SDistance_;

	//	レイダーからターゲットしたサバイバーへの向き
	VECTOR R2TDir_;

	//	誰かをターゲットしてるか
	bool isTarget_;

	//	ターゲッティングされるサバイバーのNo
	int targetSurvivorNo_;

	std::vector<std::shared_ptr<ShotBase>> shot_;

	//	待機フレーム(隙)
	float waitFlame_;

};
