#pragma once
#include <array>
#include "SceneBase.h"
class Stage;
class SkyDome;
class Raider;
class Survivor;

class GameScene : public SceneBase
{
public:
	
	//	�R���X�g���N�^
	GameScene(void);

	//	�f�X�g���N�^
	~GameScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	static constexpr int SURVIVOR_NUM = 3;

private:

	//	�X�e�[�W
	std::unique_ptr<Stage> stage_;

	//	�X�J�C�h�[��
	std::unique_ptr<SkyDome> skyDome_;

	//	�v���C���[
	std::shared_ptr<Raider> raider_;

	//	1~3�l�ƌ��܂��Ă���̂�vector�ɂ��Ȃ�
	std::array<std::shared_ptr<Survivor>, SURVIVOR_NUM> survivor_;
};
