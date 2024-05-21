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

	//	STATE::PLAY����STATE
	enum class STATE_INPLAY
	{
		NONE,
		IDLE,
		MOVE,
		JUMP,
		LAND,	//	���n
		FALL,	//	����ɗ������Ă�
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

	//	�R���X�g���N�^(�T�o�C�o�[�̃v���C���[�ԍ�)
	Survivor(int survivorNum);

	//	�f�X�g���N�^
	~Survivor(void);

	void Init(void) override;
	void SetParam(void) override;
	void Update(void) override;
	void Draw(void) override;

	//	�Փ˔���ɗp������R���C�_����
	void AddCollider(Collider* collider);
	void ClearCollider(void);

	//	�Փ˗p�J�v�Z���̎擾
	const Capsule* GetCapsule(void) const;
	void AddCapsule(Capsule* capsule);

	//	���݂�STATE::PLAY���X�e�[�g�����͂����X�e�[�g�Ɠ��������ׂ�
	bool IsStateInPlay(STATE_INPLAY state);

	void SetEnemy(std::weak_ptr<Transform> tran);

private:

	//	�A�j���[�V����
	AnimationController* animationController_;

	//	��ԊǗ�
	STATE state_;

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

	//	�n��A�󒆊֌W�Ȃ�STATE::PLAY���X�e�[�g�̐؂�ւ�
	void ChangeStateInPlay(STATE_INPLAY state);

	//	����
	void ProcessMove(void);
	void ProcessJump(void);

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

	std::weak_ptr<Transform> enemyTran_;

	//	�����̃v���C���[No.�T�o�C�o�[���̂�
	int plNum_;
};

