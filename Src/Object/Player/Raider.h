#pragma once
#include "PlayerBase.h"
class ShotBase;
class Survivor;
class Victim;

class Raider : public PlayerBase
{

public:

	//	スピード
	static constexpr float SPEED_RUN_LV1 = 15.0f;
	static constexpr float SPEED_RUN_LV2 = 25.0f;
	static constexpr float SPEED_FLY = 20.0f;

	//	回転完了までの時間
	static constexpr float TIME_ROT = 0.3f;

	//	ジャンプ力
	static constexpr float POW_JUMP_LV1 = 15.0f;

	static constexpr float POW_JUMP_LV2 = 200.0f;

	//	ジャンプ受付時間
	static constexpr float TIME_JUMP_IN = 0.5f;

	static constexpr int FRAME_HEAD_LV1 = 5;
	static constexpr int FRAME_HEAD_LV2 = 73;

	//	サバイバーの数
	//	GameSceneのやつを使えた方がいいけど、ヘッダーでインクルードすることになって良くない
	static constexpr int SURVIVOR_NUM = 3;

	//	処刑に必要なボタン長押しフレーム量
	static constexpr float EXECUTION_FLAME = 60;
	static constexpr float CHASE_FLAME = 90;
	static constexpr float ATTACK_FLAME = 30;

	static constexpr float ATTACK_POW = 50.0f;

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
		LAND,			//	着地
		FALL_MYSELF,	//	自分で降下してる
		FALL_NATURE,	//	勝手に落下してる
		FLOAT,			//	自分で上昇してる
		STUN,
		ATTACK,
		SHOT,
		EXECUTION_LV1,
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
		ATTACK_CHASE,
		ATTACK_HIT,
		ATTACK_END,
		ATTACK_SHOT,
		STUN,
		EXECUTION_LV1,

		IDLE_LV2,
		RUN_LV2,
		FAST_RUN_LV2,
		JUMP_LV2,
		WARP_PAUSE_LV2,
		FLY_LV2,
		FALLING_LV2,
		FLOAT_LV2,
		VICTORY_LV2,
		ATTACK_CHASE_LV2,
		ATTACK_HIT_LV2,
		ATTACK_END_LV2,
		ATTACK_SHOT_LV2,
		STUN_LV2,
		EXECUTION_LV2,
	};

	enum class ATTACK_TYPE
	{
		NONE,
		CHASE,
		HIT,
		END,
	};

	enum class EXE_TYPE
	{
		NONE,
		PREPARE,
		EXEQUTED,
		EVOLUTION
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

	void OnCollision(std::weak_ptr<Collider> collider) override;

	void DebugDraw(void);

	//	現在のSTATE::PLAY中ステートが入力したステートと同じか調べる
	bool IsStateInPlay(STATE_INPLAY state);

	//	StateがAttackか調べる(Survivorクラス用)
	bool IsAttack(void);

	bool IsAttackType(ATTACK_TYPE type);
	bool IsExeType(EXE_TYPE type);


	void SetSurvivor(std::array<std::weak_ptr<Survivor>, SURVIVOR_NUM> surv);
	void SetVictim(std::vector<std::weak_ptr<Victim>> tran);

	bool IsWaitNow(void);
			
private:

	//	ターゲット範囲の最大距離
	static constexpr float MAX_DISTANCE_TARGET = 3000.0f;
	static constexpr float MAX_DISTANCE_ATTACKTARGET = MAX_DISTANCE_TARGET / 3.0f;

	static constexpr float MAX_DISTANCE_EXECUTION = 300;

	static constexpr int MAX_EVOLUTION_POINT = 100;

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

	void ChangeAttack(ATTACK_TYPE type_);
	void ChangeExecution(EXE_TYPE type_);

	//	移動
	void ProcessMove(void) override;
	void ProcessJump(void) override;
	void ProcessFly(void);
	void ProcessMoveFly(void);

	//	攻撃
	void AttackStart(void);
	//	攻撃命中時の処理
	void AttackHit(void);
	//	攻撃終了時の処理
	void AttackEnd(void);

	//	処刑準備(ボタン長押し)
	void PrepareExecution(void);

	//	処刑
	void Execution(std::shared_ptr<Survivor> target);
	void Execution(std::shared_ptr<Victim> target);

	//	進化
	void Evolution(void);

	void EvoAnimation(LEVEL_PL lv);

	void ExeEvoUpdate();

	void MakeShot(void);

	//	弾初期化
	void ShotInit(std::shared_ptr<ShotBase> shot);

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

	void SetWaitFlame(float flame = -1);
	void WaitFlame(void);

	void BlowOff(void) override;

	virtual void KnockOuted(void) override;

	bool blowOffFlag_;

	//	現在の処刑可能対象(範囲内でサバイバー優先)
	TARGET exeTarget_;

	std::shared_ptr<Survivor> ExecuteSur_;
	std::shared_ptr<Victim> ExecuteVic_;

	float exeCnt_;

	float chaseTime_;

	float attackCnt_;

	//	プレイヤーがいるのが地上か空中かを可視化
	STATE_PLPOS statePlPos_;

	//	STATE::PLAYの中のステート
	STATE_INPLAY statePlay_;

	//	レイダーのレベル
	LEVEL_PL levelRaider_;

	//	けいけんち
	int exp_;

	//	移動スピード
	float speed_;

	//	飛翔判定
	bool isFly_;

	//	サバイバー、生贄
	std::array<std::weak_ptr<Survivor>, SURVIVOR_NUM> survivor_;
	std::vector<std::weak_ptr<Victim>> victim_;

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

	std::shared_ptr<Transform> exeQube_;

	ATTACK_TYPE attackType_;
	EXE_TYPE executionType_;

	float attackEndFlame_;
};
