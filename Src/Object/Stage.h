#pragma once
#include <map>
#include "Common/Transform.h"
class ResourceManager;
class WarpStar;
class Planet;
class Raider;
class Survivor;

class Stage
{

public:

	//	�X�e�[�W�̐؂�ւ��Ԋu
	static constexpr float TIME_STAGE_CHANGE = 1.0f;

	//	�X�e�[�W��
	enum class NAME
	{
		MAIN_PLANET,
		FALL_PLANET,
		FLAT_PLANET_BASE,
		FLAT_PLANET_ROT01,
	};

	//	�R���X�g���N�^
	Stage(void);

	//	�f�X�g���N�^
	~Stage(void);

	void Init(void);
	void Update(void);
	void Draw(void);

	void SetObject(std::weak_ptr<Raider> raider, std::array<std::weak_ptr<Survivor>,3> survivor);

	//	�X�e�[�W�ύX
	void ChangeStage(NAME type);

	//	�ΏۃX�e�[�W���擾
	Planet* GetPlanet(NAME type);

private:

	//	�V���O���g���Q��
	ResourceManager& resMng_;

	std::weak_ptr<Raider> raider_;
	std::array<std::weak_ptr<Survivor>,3> survivor_;

	//	�X�e�[�W�A�N�e�B�u�ɂȂ��Ă���f���̏��
	NAME activeName_;
	Planet* activePlanet_;

	//	�f��
	std::map<NAME, Planet*> planets_;

	//	���Planet
	Planet* nullPlanet = nullptr;

	float step_;

	//	�ŏ��̘f��
	void MakeMainStage(void);

};
