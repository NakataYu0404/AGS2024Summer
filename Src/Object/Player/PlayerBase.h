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

	//	���
	enum class STATE
	{
		NONE,
		PLAY,
		DEAD,
		VICTORY,
		END
	};

	static constexpr float DEFAULT_GRAVITY_POW = 1.0f;

	//	�R���X�g���N�^
	PlayerBase(void);

	//	�f�X�g���N�^
	~PlayerBase(void);

	//	�Փ˔���ɗp������R���C�_����
	void AddCollider(std::shared_ptr<Collider> collider);
	void ClearCollider(void);

	void AddCapsule(std::shared_ptr<Capsule> capsule);

	float GetGravity(void);

	//	vec������pow�̐����Ő������ stunTime���I���ƕ��A
	void SetBlowOff(VECTOR vec, float pow, float stunTime);	//	������΂��Ώ�.SetBlowOff(vec...)���Ċ����Ŏg��

protected:

	//	��ԑJ��
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStatePlay(void);

	//	Anim������
	virtual void InitAnimation(void) = 0;

	//	���݂�STATE�ɍ��킹�ĕς��A�j���[�V����
	virtual void ChangeStateAnimation(void) = 0;

	//	�X�V�X�e�b�v
	virtual void UpdateNone(void);
	virtual void UpdatePlay(void) = 0;

	//	�`��n
	void DrawShadow(void);

	//	����
	virtual void ProcessMove(void) = 0;
	virtual void ProcessJump(void) = 0;

	//	��]
	virtual void SetGoalRotate(double rotRad) = 0;
	void Rotate(void);

	//	�Փ˔���
	void Collision(void);
	virtual void CollisionGravity(void);

	//	�ړ��ʂ̌v�Z
	virtual void CalcGravityPow(void) = 0;

	//	���n���[�V�����I��
	virtual bool IsEndLanding(void) = 0;

	//	����������Transform��������(�����Ő��K���A�l�𐳂̐��ɂ��܂�)
	float Myself2OtherDistance(std::weak_ptr<Transform> toTran);

	virtual void BlowOff(void) = 0;

	//	��]�����܂ł̎���
	static constexpr float TIME_ROT = 0.3f;

	//	�A�j���[�V����
	std::shared_ptr<AnimationController> animationController_;

	//	�Փ˔���ɗp������R���C�_
	std::vector<std::shared_ptr<Collider>> colliders_;

	//	�ړ��X�s�[�h
	float speed_;

	//	��ԊǗ�
	STATE state_;

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

	//	�W�����v�̓��͎�t����
	float stepJump_;

	//	�Փ˃`�F�b�N
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	float gravityPow_;

	//	�ۉe
	int imgShadow_;

	//	�V���O���g���Q��
	CollisionManager& colMng_;

	VECTOR blowOffVec_;
	float blowOffPow_;
	float stunTime_;
};
