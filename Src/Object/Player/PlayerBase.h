#pragma once
#include <map>
#include <vector>
#include <array>
#include "../ActorBase.h"
class AnimationController;
class Collider;
class Capsule;

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

	//	�R���X�g���N�^
	PlayerBase(void);

	//	�f�X�g���N�^
	~PlayerBase(void);

	//	�Փ˔���ɗp������R���C�_����
	void AddCollider(Collider* collider);
	void ClearCollider(void);

	//	�Փ˗p�J�v�Z���̎擾
	const std::weak_ptr<Capsule> GetCapsule(void) const;
	void AddCapsule(Capsule* capsule);

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
	virtual void CollisionGravity(void) = 0;
	void CollisionCapsule(void);

	//	�ړ��ʂ̌v�Z
	virtual void CalcGravityPow(void) = 0;

	//	���n���[�V�����I��
	virtual bool IsEndLanding(void) = 0;

	//	����������Transform��������
	float Myself2OtherDistance(std::weak_ptr<Transform> toTran);

	//	��]�����܂ł̎���
	static constexpr float TIME_ROT = 0.3f;

	//	�A�j���[�V����
	std::shared_ptr<AnimationController> animationController_;

	//	�Փ˔���ɗp������R���C�_
	std::vector<Collider*> colliders_;
	std::shared_ptr<Capsule> capsule_;

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

};
