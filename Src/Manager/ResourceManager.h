#pragma once
#include <map>
#include <string>
#include "Resource.h"

class ResourceManager
{

public:

	//	リソース名
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

	//	明示的にインステンスを生成する
	static void CreateInstance(void);

	//	静的インスタンスの取得
	static ResourceManager& GetInstance(void);

	//	初期化
	void Init(void);

	//	解放(シーン切替時に一旦解放)
	void Release(void);

	//	リソースの完全破棄
	void Destroy(void);

	//	リソースのロード
	const Resource& Load(SRC src);

	//	リソースの複製ロード(モデル用)
	int LoadModelDuplicate(SRC src);

private:

	//	静的インスタンス
	static ResourceManager* instance_;

	//	リソース管理の対象
	std::map<SRC, Resource*> resourcesMap_;

	//	読み込み済みリソース
	std::map<SRC, Resource&> loadedMap_;

	Resource dummy_;

	//	デフォルトコンストラクタをprivateにして、
	//	外部から生成できない様にする
	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = default;
	~ResourceManager(void) = default;

	//	内部ロード
	Resource& _Load(SRC src);

};
