#pragma once
#include <map>
#include <vector>
#include <array>
#include <memory>
#include "../ActorBase.h"
class AnimationController;
class Collider;
class Capsule;
class CollisionManager;

class PlayerBase : public ActorBase
{

public:

	//	状態
	enum class STATE
	{
		NONE,
		PLAY,
		DEAD,
		VICTORY,
		END
	};

	static constexpr float DEFAULT_GRAVITY_POW = 1.0f;

	//	コンストラクタ
	PlayerBase(void);

	//	デストラクタ
	~PlayerBase(void);

	//	衝突判定に用いられるコライダ制御
	void AddCollider(std::shared_ptr<Collider> collider);
	void ClearCollider(void);

	void AddCapsule(std::shared_ptr<Capsule> capsule);

	float GetGravity(void);

	//	vec方向にpowの勢いで吹っ飛ぶ stunTimeが終わると復帰
	void SetBlowOff(VECTOR vec, float pow, float stunTime);	//	吹っ飛ばす対象.SetBlowOff(vec...)って感じで使う

protected:

	//	状態遷移
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStatePlay(void);

	//	Anim初期化
	virtual void InitAnimation(void) = 0;

	//	現在のSTATEに合わせて変わるアニメーション
	virtual void ChangeStateAnimation(void) = 0;

	//	更新ステップ
	virtual void UpdateNone(void);
	virtual void UpdatePlay(void) = 0;

	//	描画系
	void DrawShadow(void);

	//	操作
	virtual void ProcessMove(void) = 0;
	virtual void ProcessJump(void) = 0;

	//	回転
	virtual void SetGoalRotate(double rotRad) = 0;
	void Rotate(void);

	//	衝突判定
	void Collision(void);
	virtual void CollisionGravity(void);

	//	移動量の計算
	virtual void CalcGravityPow(void) = 0;

	//	着地モーション終了
	virtual bool IsEndLanding(void) = 0;

	//	自分→何かTransform直線距離(内部で正規化、値を正の数にします)
	float Myself2OtherDistance(std::weak_ptr<Transform> toTran);

	virtual void BlowOff(void) = 0;

	//	回転完了までの時間
	static constexpr float TIME_ROT = 0.3f;

	//	アニメーション
	std::shared_ptr<AnimationController> animationController_;

	//	衝突判定に用いられるコライダ
	std::vector<std::shared_ptr<Collider>> colliders_;

	//	移動スピード
	float speed_;

	//	状態管理
	STATE state_;

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

	//	ジャンプの入力受付時間
	float stepJump_;

	//	衝突チェック
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	float gravityPow_;

	//	丸影
	int imgShadow_;

	//	シングルトン参照
	CollisionManager& colMng_;

	VECTOR blowOffVec_;
	float blowOffPow_;
	float stunTime_;
};
