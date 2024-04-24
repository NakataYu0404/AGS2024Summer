#pragma once
#include <map>
#include <DxLib.h>
#include "ActorBase.h"
class AnimationController;
class Collider;
class Capsule;

class Player : public ActorBase
{

public:

	//	�X�s�[�h
	static constexpr float SPEED_RUN = 15.0f;
	static constexpr float SPEED_FLY = 20.0f;

	//	��]�����܂ł̎���
	static constexpr float TIME_ROT = 0.3f;

	//	�W�����v��
	static constexpr float POW_JUMP = 15.0f;

	//	�W�����v��t����
	static constexpr float TIME_JUMP_IN = 0.5f;

	//	���
	enum class STATE
	{
		NONE,
		PLAY,
		DEAD,
		VICTORY,
		END
	};
	
	//	�󒆂ɂ��������n��ɂ��������ŏ�����ς�����(�s�R�͂ŋ󒆂ɂ��Ă��܂��Ă�ꍇ��(�����Ƃ�)LAND�ŁA�������畂���Ă�Ƃ���AIR)
	enum class STATE_PLPOS
	{
		NONE,
		LAND,
		AIR,
	};

	//	STATE::PLAY����STATE
	enum class STATE_INPLAY
	{
		NONE,
		IDLE,
		MOVE,
		JUMP,
		LAND,	//	���n
		FALL,	//	�����ō~�����Ă�
		FLOAT,	//	�����ŏ㏸���Ă�
		ATTACK,
		SHOT,
		STUN,
	};

	//	�A�j���[�V�������
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

	//	�R���X�g���N�^
	Player(void);

	//	�f�X�g���N�^
	~Player(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	//	�Փ˔���ɗp������R���C�_����
	void AddCollider(Collider* collider);
	void ClearCollider(void);

	//	�Փ˗p�J�v�Z���̎擾
	const Capsule* GetCapsule(void) const;

	//	���݂�STATE::PLAY���X�e�[�g�����͂����X�e�[�g�Ɠ��������ׂ�
	bool IsStateInPlay(STATE_INPLAY state);
private:

	//	�A�j���[�V����
	AnimationController* animationController_;

	//	��ԊǗ�
	STATE state_;

	//	�v���C���[������̂��n�ォ�󒆂�������
	STATE_PLPOS statePlPos_;

	//	STATE::PLAY�̒��̃X�e�[�g
	STATE_INPLAY statePlay_;

	//	�ړ��X�s�[�h
	float speed_;

	//	�ړ�����
	VECTOR moveDir_;

	//	�ړ���
	VECTOR movePow_;

	//	�ړ���̍��W
	VECTOR movedPos_;

	//	��]
	Quaternion playerRotY_;
	Quaternion goalQuaRot_;
	float stepRotTime_;

	float rotRad_;

	//	�W�����v��
	VECTOR jumpPow_;

	//	�W�����v����
	bool isJump_;

	//	���Ĕ���
	bool isFly_;

	//	�W�����v�̓��͎�t����
	float stepJump_;

	//	�Փ˔���ɗp������R���C�_
	std::vector<Collider*> colliders_;
	Capsule* capsule_;

	//	�Փ˃`�F�b�N
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	float gravityPow_;

	//	�ۉe
	int imgShadow_;

	void InitAnimation(void);

	//	��ԑJ��
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStatePlay(void);

	//	���݂�STATE�ɍ��킹�ĕς��A�j���[�V����
	void ChangeStateAnimation(void);

	//	�X�V�X�e�b�v
	void UpdateNone(void);
	void UpdatePlay(void);
	
	//	�`��n
	void DrawShadow(void);

	//	�n��A�󒆕ʂ̏���
	void UpdateLand(void);
	void UpdateAir(void);

	//	�n��A�󒆂̐؂�ւ�
	void ChangeLandAir(void);

	//	�n��A�󒆊֌W�Ȃ�STATE::PLAY���X�e�[�g�̐؂�ւ�
	void ChangeStateInPlay(STATE_INPLAY state);

	//	����
	void ProcessMove(void);
	void ProcessJump(void);
	void ProcessFly(void);
	void ProcessMoveFly(void);

	//	��]
	void SetGoalRotate(double rotRad);
	void Rotate(void);

	//	�Փ˔���
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);

	//	�ړ��ʂ̌v�Z
	void CalcGravityPow(void);

	//	���n���[�V�����I��
	bool IsEndLanding(void);

	Transform followTran_;
};
