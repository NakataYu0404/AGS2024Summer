#pragma once
#include "PlayerBase.h"
class ShotBase;
class Survivor;
class Victim;

class Raider : public PlayerBase
{

public:

	//	�X�s�[�h
	static constexpr float SPEED_RUN_LV1 = 15.0f;
	static constexpr float SPEED_RUN_LV2 = 25.0f;
	static constexpr float SPEED_FLY = 20.0f;

	//	��]�����܂ł̎���
	static constexpr float TIME_ROT = 0.3f;

	//	�W�����v��
	static constexpr float POW_JUMP_LV1 = 15.0f;

	static constexpr float POW_JUMP_LV2 = 200.0f;

	//	�W�����v��t����
	static constexpr float TIME_JUMP_IN = 0.5f;

	static constexpr int FRAME_HEAD_LV1 = 5;
	static constexpr int FRAME_HEAD_LV2 = 73;

	//	�T�o�C�o�[�̐�
	//	GameScene�̂���g���������������ǁA�w�b�_�[�ŃC���N���[�h���邱�ƂɂȂ��ėǂ��Ȃ�
	static constexpr int SURVIVOR_NUM = 3;

	//	���Y�ɕK�v�ȃ{�^���������t���[����
	static constexpr float EXECUTION_FLAME = 60;
	static constexpr float CHASE_FLAME = 90;
	static constexpr float ATTACK_FLAME = 30;

	static constexpr float ATTACK_POW = 50.0f;

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
		LAND,			//	���n
		FALL_MYSELF,	//	�����ō~�����Ă�
		FALL_NATURE,	//	����ɗ������Ă�
		FLOAT,			//	�����ŏ㏸���Ă�
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

	//	�R���X�g���N�^
	Raider(void);

	//	�f�X�g���N�^
	~Raider(void);

	void Init(void) override;
	void SetParam(void) override;
	void Update(void) override;
	void Draw(void) override;

	void OnCollision(std::weak_ptr<Collider> collider) override;

	void DebugDraw(void);

	//	���݂�STATE::PLAY���X�e�[�g�����͂����X�e�[�g�Ɠ��������ׂ�
	bool IsStateInPlay(STATE_INPLAY state);

	//	State��Attack�����ׂ�(Survivor�N���X�p)
	bool IsAttack(void);

	bool IsAttackType(ATTACK_TYPE type);
	bool IsExeType(EXE_TYPE type);


	void SetSurvivor(std::array<std::weak_ptr<Survivor>, SURVIVOR_NUM> surv);
	void SetVictim(std::vector<std::weak_ptr<Victim>> tran);

	bool IsWaitNow(void);
			
private:

	//	�^�[�Q�b�g�͈͂̍ő勗��
	static constexpr float MAX_DISTANCE_TARGET = 3000.0f;
	static constexpr float MAX_DISTANCE_ATTACKTARGET = MAX_DISTANCE_TARGET / 3.0f;

	static constexpr float MAX_DISTANCE_EXECUTION = 300;

	static constexpr int MAX_EVOLUTION_POINT = 100;

	//	Anim������
	void InitAnimation(void) override;

	//	���݂�STATE�ɍ��킹�ĕς��A�j���[�V����
	void ChangeStateAnimation(void) override;

	//	�X�V�X�e�b�v
	void UpdatePlay(void) override;

	//	�e�̍X�V
	void UpdateShot(void);

	//	�`��n
	void DrawShot(void);

	//	�n��A�󒆕ʂ̏���
	void UpdateLand(void);
	void UpdateAir(void);

	//	�n��A�󒆂̐؂�ւ�
	void ChangeLandAir(void);

	//	�n��A�󒆊֌W�Ȃ�STATE::PLAY���X�e�[�g�̐؂�ւ�
	void ChangeStateInPlay(STATE_INPLAY state);

	//	���ł邩�t���O�������Őݒ肷��(���݃��x���ɂ���ĕς��邩�ǂ������߂���)
	void ChangeIsFly(bool isFly);

	void ChangeAttack(ATTACK_TYPE type_);
	void ChangeExecution(EXE_TYPE type_);

	//	�ړ�
	void ProcessMove(void) override;
	void ProcessJump(void) override;
	void ProcessFly(void);
	void ProcessMoveFly(void);

	//	�U��
	void AttackStart(void);
	//	�U���������̏���
	void AttackHit(void);
	//	�U���I�����̏���
	void AttackEnd(void);

	//	���Y����(�{�^��������)
	void PrepareExecution(void);

	//	���Y
	void Execution(std::shared_ptr<Survivor> target);
	void Execution(std::shared_ptr<Victim> target);

	//	�i��
	void Evolution(void);

	void EvoAnimation(LEVEL_PL lv);

	void ExeEvoUpdate();

	void MakeShot(void);

	//	�e������
	void ShotInit(std::shared_ptr<ShotBase> shot);

	//	��]
	void SetGoalRotate(double rotRad) override;

	//	�Փ˔���
	void CollisionGravity(void) override;

	//	�ړ��ʂ̌v�Z
	void CalcGravityPow(void) override;

	//	���n���[�V�����I��
	bool IsEndLanding(void) override;

	//	���b�N�I��
	void LockOn(void);

	//	true�Ń^�[�Q�b�e�B���O
	bool CanTarget(int num);
	VECTOR ShotDir(void);

	void SetWaitFlame(float flame = -1);
	void WaitFlame(void);

	void BlowOff(void) override;

	virtual void KnockOuted(void) override;

	bool blowOffFlag_;

	//	���݂̏��Y�\�Ώ�(�͈͓��ŃT�o�C�o�[�D��)
	TARGET exeTarget_;

	std::shared_ptr<Survivor> ExecuteSur_;
	std::shared_ptr<Victim> ExecuteVic_;

	float exeCnt_;

	float chaseTime_;

	float attackCnt_;

	//	�v���C���[������̂��n�ォ�󒆂�������
	STATE_PLPOS statePlPos_;

	//	STATE::PLAY�̒��̃X�e�[�g
	STATE_INPLAY statePlay_;

	//	���C�_�[�̃��x��
	LEVEL_PL levelRaider_;

	//	��������
	int exp_;

	//	�ړ��X�s�[�h
	float speed_;

	//	���Ĕ���
	bool isFly_;

	//	�T�o�C�o�[�A����
	std::array<std::weak_ptr<Survivor>, SURVIVOR_NUM> survivor_;
	std::vector<std::weak_ptr<Victim>> victim_;

	//	���C�_�[����T�o�C�o�[�ւ̋���
	std::array<float, SURVIVOR_NUM> R2SDistance_;

	//	���C�_�[����^�[�Q�b�g�����T�o�C�o�[�ւ̌���
	VECTOR R2TDir_;

	//	�N�����^�[�Q�b�g���Ă邩
	bool isTarget_;

	//	�^�[�Q�b�e�B���O�����T�o�C�o�[��No
	int targetSurvivorNo_;

	std::vector<std::shared_ptr<ShotBase>> shot_;

	//	�ҋ@�t���[��(��)
	float waitFlame_;

	std::shared_ptr<Transform> exeQube_;

	ATTACK_TYPE attackType_;
	EXE_TYPE executionType_;

	float attackEndFlame_;
};
