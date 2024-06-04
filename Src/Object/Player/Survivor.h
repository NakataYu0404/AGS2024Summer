#pragma once
#include "PlayerBase.h"

class Survivor : public PlayerBase
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

	static constexpr int FRAME_HEAD = 6;

	//	���C�_�[���E�������ɓ���|�C���g 100�Ői��
	static constexpr int POINT_EVOLUTION = 50;

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
		CRAWL,	//	���ɂ����Ŕ����Ă���
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


	//	���݂�STATE::PLAY���X�e�[�g�����͂����X�e�[�g�Ɠ��������ׂ�
	bool IsStateInPlay(STATE_INPLAY state);

	void SetRaider(std::weak_ptr<Transform> tran);

	Survivor::STATE_INPLAY GetStatePlay(void);

	void SetState(PlayerBase::STATE state);

	//	�n��A�󒆊֌W�Ȃ�STATE::PLAY���X�e�[�g�̐؂�ւ�
	void ChangeStateInPlay(STATE_INPLAY state);

	void OnCollision(std::weak_ptr<Collider> collider) override;

private:


	void InitAnimation(void) override;

	//	���݂�STATE�ɍ��킹�ĕς��A�j���[�V����
	void ChangeStateAnimation(void) override;

	//	�X�V�X�e�b�v
	void UpdatePlay(void) override;

	//	�n��A�󒆕ʂ̏���
	void UpdateLand(void);


	//	����
	void ProcessMove(void) override;
	void ProcessJump(void) override;

	//	��]
	void SetGoalRotate(double rotRad) override;
	void CollisionGravity(void) override;

	//	�ړ��ʂ̌v�Z
	void CalcGravityPow(void) override;

	//	���n���[�V�����I��
	bool IsEndLanding(void) override;

	void BlowOff(void) override;
	void Stun(void);

	//	�����̃v���C���[No.�T�o�C�o�[���̂�
	int plNum_;

	//	STATE::PLAY�̒��̃X�e�[�g
	STATE_INPLAY statePlay_;

	//	���Ĕ���
	bool isFly_;

	//	���C�_�[��Transform
	std::weak_ptr<Transform> raiderTran_;

};

