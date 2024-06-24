#pragma once
#include <map>
#include <string>
#include "Resource.h"

class ResourceManager
{

public:

	//	���\�[�X��
	enum class SRC
	{
		IMG_TITLE,
		IMG_PUSHSPACE,
		IMG_SPEECHBALLOON,
		MDL_PLAYER,
		MDL_RAIDER_LV1,
		MDL_RAIDER_LV2,
		MDL_RAIDER_LV3,
		MDL_SURVIVOR,
		IMG_PLAYERSHADOW,
		MDL_SKYDOME,
		MDL_MAINPLANET,
		MDL_FALLPLANET,
		MDL_LASTPLANET,
		FOOT_SMOKE,
		BLACK_HOLE,
		GOAL_STAR,
		CLEAR,
		SHOT,
		EXEQUBE,
	};

	//	�����I�ɃC���X�e���X�𐶐�����
	static void CreateInstance(void);

	//	�ÓI�C���X�^���X�̎擾
	static ResourceManager& GetInstance(void);

	//	������
	void Init(void);

	//	���(�V�[���ؑ֎��Ɉ�U���)
	void Release(void);

	//	���\�[�X�̊��S�j��
	void Destroy(void);

	//	���\�[�X�̃��[�h
	const Resource& Load(SRC src);

	//	���\�[�X�̕������[�h(���f���p)
	int LoadModelDuplicate(SRC src);

private:

	//	�ÓI�C���X�^���X
	static ResourceManager* instance_;

	//	���\�[�X�Ǘ��̑Ώ�
	std::map<SRC, Resource*> resourcesMap_;

	//	�ǂݍ��ݍς݃��\�[�X
	std::map<SRC, Resource&> loadedMap_;

	Resource dummy_;

	//	�f�t�H���g�R���X�g���N�^��private�ɂ��āA
	//	�O�����琶���ł��Ȃ��l�ɂ���
	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = default;
	~ResourceManager(void) = default;

	//	�������[�h
	Resource& _Load(SRC src);

};
