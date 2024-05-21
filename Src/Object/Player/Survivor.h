#pragma once
#include <map>
#include <vector>
#include "../ActorBase.h"
class AnimationController;
class Collider;
class Capsule;

class Survivor : public ActorBase
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

	//	コンストラクタ(サバイバーのプレイヤー番号)
	Survivor(int survivorNum);

	//	デストラクタ
	~Survivor(void);

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

	void SetEnemy(std::weak_ptr<Transform> tran);

private:

	//	アニメーション
	AnimationController* animationController_;

	//	状態管理
	STATE state_;

	//	STATE::PLAYの中のステート
	STATE_INPLAY statePlay_;


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

	//	地上、空中関係ないSTATE::PLAY中ステートの切り替え
	void ChangeStateInPlay(STATE_INPLAY state);

	//	操作
	void ProcessMove(void);
	void ProcessJump(void);

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

	std::weak_ptr<Transform> enemyTran_;

	//	自分のプレイヤーNo.サバイバー内のみ
	int plNum_;
};

