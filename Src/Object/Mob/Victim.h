#pragma once
#include "../ActorBase.h"

class AnimationController;


class Victim : public ActorBase
{
public:

	//	���C�_�[���E�������ɓ���|�C���g 100�Ői��
	static constexpr int POINT_EVOLUTION = 25;

	//	���
	enum class STATE
	{
		NONE,
		PLAY,
		DEAD,
		END
	};

	//	�A�j���[�V�������
	enum class ANIM_TYPE
	{
		IDLE,
		DEAD,
	};


	Victim();
	~Victim();

	void Init() override;
	void Update() override;
	void Draw() override;
	void SetParam() override;

private:

	void InitAnimation(void);

	//	��ԑJ��
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStatePlay(void);

	//	�X�V�X�e�b�v
	void UpdateNone(void);
	void UpdatePlay(void);

	//	�`��n
	void DrawShadow(void);

	//	�����Ă邩
	bool isAlive_;

	//	�A�j���[�V����
	AnimationController* animationController_;

	//	��ԊǗ�
	STATE state_;

	//	�ۉe
	int imgShadow_;


};

