#pragma once
#include <map>
#include <vector>
#include <DxLib.h>
#include "../ActorBase.h"
class AnimationController;
class Collider;
class Capsule;

class Raider : public ActorBase
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

	//	状態
	enum class STATE
	{
		NONE,
		PLAY,
		DEAD,
		VICTORY,
		END
	};
	
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
	};

	enum class STATE_ATTACK
	{
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

	//	コンストラクタ
	Raider(void);

	//	デストラクタ
	~Raider(void);

	void Init(void) override;
	void SetParam(void) override;
	void Update(void) override;
	void Draw(void) override;

	//	衝突判定に用いられるコライダ制御
	void AddCollider(Collider* collider);
	void ClearCollider(void);

	//	衝突用カプセルの取得
	const Capsule* GetCapsule(void) const;
	void AddCapsule(Capsule* capsule);

	//	現在のSTATE::PLAY中ステートが入力したステートと同じか調べる
	bool IsStateInPlay(STATE_INPLAY state);

	void SetEnemy(std::weak_ptr<Transform> tran[3]);
private:

	//	アニメーション
	AnimationController* animationController_;

	//	状態管理
	STATE state_;

	//	プレイヤーがいるのが地上か空中かを可視化
	STATE_PLPOS statePlPos_;

	//	STATE::PLAYの中のステート
	STATE_INPLAY statePlay_;

	LEVEL_PL levelRaider_;

	//	移動スピード
	float speed_;

	//	移動方向
	VECTOR moveDir_;

	//	移動量
	VECTOR movePow_;

	//	移動後の座標
	VECTOR movedPos_;

	//	回転
	Quaternion playerRotY_;
	Quaternion goalQuaRot_;
	float stepRotTime_;

	float rotRad_;

	//	ジャンプ量
	VECTOR jumpPow_;

	//	ジャンプ判定
	bool isJump_;

	//	飛翔判定
	bool isFly_;

	//	ジャンプの入力受付時間
	float stepJump_;

	//	衝突判定に用いられるコライダ
	std::vector<Collider*> colliders_;
	Capsule* capsule_;

	//	衝突チェック
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	float gravityPow_;

	//	丸影
	int imgShadow_;

	void InitAnimation(void);

	//	状態遷移
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStatePlay(void);

	//	現在のSTATEに合わせて変わるアニメーション
	void ChangeStateAnimation(void);

	//	更新ステップ
	void UpdateNone(void);
	void UpdatePlay(void);
	
	//	描画系
	void DrawShadow(void);

	//	地上、空中別の処理
	void UpdateLand(void);
	void UpdateAir(void);

	//	地上、空中の切り替え
	void ChangeLandAir(void);

	//	地上、空中関係ないSTATE::PLAY中ステートの切り替え
	void ChangeStateInPlay(STATE_INPLAY state);

	//	飛んでるかフラグをここで設定する(現在レベルによって変えるかどうか決められる)
	void ChangeIsFly(bool isFly);

	//	操作
	void ProcessMove(void);
	void ProcessJump(void);
	void ProcessFly(void);
	void ProcessMoveFly(void);

	//	回転
	void SetGoalRotate(double rotRad);
	void Rotate(void);

	//	衝突判定
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);

	//	移動量の計算
	void CalcGravityPow(void);

	//	着地モーション終了
	bool IsEndLanding(void);

	std::weak_ptr<Transform> enemyTran_[3];

	//	ロックオン
	void LockOn(void);

	//	キャラクターの距離(軸を考えない)
	float CheckDistance(int num);

	//	trueでターゲッティング
	bool IsTarget(int num);

	//	レイダーからサバイバーへの距離
	float R2SDistance_[3];
};
