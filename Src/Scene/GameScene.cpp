#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/SkyDome.h"
#include "../Object/Stage.h"
#include "../Object/Player/Raider.h"
#include "../Object/Planet.h"
#include "../Object/Player/Survivor.h"
#include "GameScene.h"

GameScene::GameScene(void)
{
	skyDome_ = nullptr;
	stage_ = nullptr;
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	//	レイダー
	raider_ = std::make_shared<Raider>();
	raider_->Init();
	
	//	サバイバー
	std::weak_ptr<Transform> raiderTran = raider_->GetTransform();
	std::weak_ptr<Transform> surviveTran[3];

	int i = 0;
	for (int i = 0; i < 3; i++)
	{
		survivor_[i] = std::make_shared<Survivor>();
		survivor_[i]->Init();
		surviveTran[i] = survivor_[i]->GetTransform();
		survivor_[i]->SetEnemy(raiderTran);
	}

	raider_->SetEnemy(surviveTran);

	//	ステージ
	//	TODO:uniqueポインタにはシェアードポインタの配列をそのまま渡せないらしい。arrayを使うんだと
	stage_ = std::make_unique<Stage>();
	stage_->SetObject(raider_, survivor_[0], survivor_[1], survivor_[2]);
	stage_->Init();

	//	ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	//	スカイドーム
	skyDome_ = std::make_unique<SkyDome>();
	skyDome_->Init();

	SceneManager::GetInstance().GetCamera()->SetFollow(raider_->GetTransform().lock().get());
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FOLLOW);
}

void GameScene::Update(void)
{
	//	シーン遷移
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_NUMPAD0))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	skyDome_->Update();

	stage_->Update();

	raider_->Update();
	for (int i = 0; i < 3; i++)
	{
		survivor_[i]->Update();
	}
}

void GameScene::Draw(void)
{
	//	背景
	skyDome_->Draw();
	stage_->Draw();
	
	raider_->Draw();
	for (int i = 0; i < 3; i++)
	{
		survivor_[i]->Draw();
	}

	//	ヘルプ
	DrawFormatString(840, 20, 0x000000, "移動　　：WASD");
	DrawFormatString(840, 40, 0x000000, "カメラ　：矢印キー");
	DrawFormatString(840, 60, 0x000000, "ダッシュ：右Shift");
	DrawFormatString(840, 80, 0x000000, "ジャンプ：＼(バクスラ)");
}
