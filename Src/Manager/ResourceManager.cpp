#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Init();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Init(void)
{

	//	推奨しませんが、どうしても使いたい方は
	using RES = Resource;
	using RES_T = RES::TYPE;
	static std::string PATH_IMG = Application::PATH_IMAGE;
	static std::string PATH_MDL = Application::PATH_MODEL;
	static std::string PATH_EFF = Application::PATH_EFFECT;

	Resource* res;
	
	//	タイトル画像
	res = new RES(RES_T::IMG, PATH_IMG + "Title.png");
	resourcesMap_.emplace(SRC::IMG_TITLE, res);
	
	//	PushSpace
	res = new RES(RES_T::IMG, PATH_IMG + "PushSpace.png");
	resourcesMap_.emplace(SRC::IMG_PUSHSPACE, res);

	//	吹き出し
	res = new RES(RES_T::IMG, PATH_IMG + "SpeechBalloon.png");
	resourcesMap_.emplace(SRC::IMG_SPEECHBALLOON, res);
	
	//	プレイヤー
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/Victim/Player.mv1");
	resourcesMap_.emplace(SRC::MDL_PLAYER, res);

	//	レイダー
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/Raider/Lv1Raider.mv1");
	resourcesMap_.emplace(SRC::MDL_RAIDER_LV1, res);
	//	
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/Raider/Lv2Raider.mv1");
	resourcesMap_.emplace(SRC::MDL_RAIDER_LV2, res);
	//	
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/BotX.mv1");
	resourcesMap_.emplace(SRC::MDL_RAIDER_LV3, res);

	//	サバイバー
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/Survivor/BotY.mv1");
	resourcesMap_.emplace(SRC::MDL_SURVIVOR, res);

	//	
	res = new RES(RES_T::MODEL, PATH_MDL + "exeSphere.mv1");
	resourcesMap_.emplace(SRC::EXEQUBE, res);

	//	プレイヤー影
	res = new RES(RES_T::IMG, PATH_IMG + "Shadow.png");
	resourcesMap_.emplace(SRC::IMG_PLAYERSHADOW, res);

	//	スカイドーム
	res = new RES(RES_T::MODEL, PATH_MDL + "SkyDome/SkyDome.mv1");
	resourcesMap_.emplace(SRC::MDL_SKYDOME, res);

	//	最初の惑星
	res = new RES(RES_T::MODEL, PATH_MDL + "Planet/MainPlanet.mv1");
	resourcesMap_.emplace(SRC::MDL_MAINPLANET, res);

	//	足煙
	res = new RES(RES_T::EFFEKSEER, PATH_EFF + "Smoke/Smoke.efkefc");
	resourcesMap_.emplace(SRC::FOOT_SMOKE, res);

	//	ブラックホール
	res = new RES(RES_T::EFFEKSEER, PATH_EFF + "BlackHole/BlackHole.efkefc");
	resourcesMap_.emplace(SRC::BLACK_HOLE, res);

	//	ゴール
	res = new RES(RES_T::MODEL, PATH_MDL + "GoalStar/GoalStar.mv1");
	resourcesMap_.emplace(SRC::GOAL_STAR, res);

	//	Clear
	res = new RES(RES_T::IMG, PATH_IMG + "Congratulations.png");
	resourcesMap_.emplace(SRC::CLEAR, res);

	res = new RES(RES_T::MODEL, PATH_MDL + "Shot/Shot.mv1");
	resourcesMap_.emplace(SRC::SHOT, res);
}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second.Release();
	}

	loadedMap_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();
	for (auto& res : resourcesMap_)
	{
		res.second->Release();
		delete res.second;
	}
	resourcesMap_.clear();
	delete instance_;
}

const Resource& ResourceManager::Load(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

ResourceManager::ResourceManager(void)
{
}

Resource& ResourceManager::_Load(SRC src)
{

	//	ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return *resourcesMap_.find(src)->second;
	}

	//	リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		//	登録されていない
		return dummy_;
	}

	//	ロード処理
	rPair->second->Load();

	//	念のためコピーコンストラクタ
	loadedMap_.emplace(src, *rPair->second);

	return *rPair->second;

}
