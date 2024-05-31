#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/Common/CollisionManager.h"
#include "../Object/SkyDome.h"
#include "../Object/Stage.h"
#include "../Object/Player/Raider.h"
#include "../Object/Player/Survivor.h"
#include "../Object/Mob/Victim.h"
#include "GameScene.h"

GameScene::GameScene(void) : colMng_(CollisionManager::GetInstance())
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
	
	//	サバイバー作成
	std::weak_ptr<Transform> raiderTran = raider_->GetTransform();
	std::array<std::weak_ptr<Survivor>, SURVIVOR_NUM> surviveWeakArray;

	for (int i = 0; i < survivors_.size(); i++)
	{
		survivors_[i] = std::make_shared<Survivor>(i);
		survivors_[i]->Init();
		surviveWeakArray[i] = survivors_[i];
		survivors_[i]->SetRaider(raiderTran);
	}

	//	生贄作成(TODO:とりあえず一人だけ作るけど、あとでＪＳＯＮ使っていっぱい用意する)	
	std::vector<std::weak_ptr<Victim>> VictimWeak;

	std::shared_ptr<Victim> tmp = std::make_shared<Victim>();
	victims_.push_back(tmp);
	for (int i = 0; i < victims_.size(); i++)
	{
		if (victims_[i] == nullptr)
		{
			break;
		}
		victims_[i]->Init();
		VictimWeak.push_back(victims_[i]);
	}
	
	//	レイダーにサバイバー、生贄情報を渡す
	raider_->SetSurvivor(surviveWeakArray);
	raider_->SetVictim(VictimWeak);

	//	ステージ
	stage_ = std::make_shared<Stage>();
	
	//	ステージに対して当たり判定付けるオブジェクトを渡すんだけど、sharedptrを配列にしちゃうと、渡すとき勝手にweakptrに変換してくれなくなっちゃうみたい。
	//	なので、こっちでweak型の配列に移し替えて、それを渡すことにする
	std::array<std::weak_ptr<Survivor>, SURVIVOR_NUM>tmpSurvivor;
	for (int i = 0; i < survivors_.size(); i++)
	{
		tmpSurvivor[i] = survivors_[i];
	}
	stage_->SetObject(raider_, tmpSurvivor);
	stage_->Init();

	//	スカイドーム
	skyDome_ = std::make_unique<SkyDome>();
	skyDome_->Init();

	colMng_.Init();
	
	colMng_.Add(raider_);
	colMng_.Add(survivors_[0]);
	colMng_.Add(survivors_[1]);
	colMng_.Add(survivors_[2]);
	colMng_.Add(stage_);

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

	for (auto& s : survivors_)
	{
		if (s == nullptr)
		{
			break;
		}
		s->Update();
	}

	for (auto& v : victims_)
	{
		if (v == nullptr)
		{
			break;
		}
		v->Update();
	}

	colMng_.Update();
}

void GameScene::Draw(void)
{
	//	背景
	skyDome_->Draw();
	stage_->Draw();
	
	raider_->Draw();

	for (auto& s : survivors_)
	{
		if (s == nullptr)
		{
			break;
		}
		s->Draw();
	}

	for (auto& v : victims_)
	{
		if (v == nullptr)
		{
			break;
		}
		v->Draw();
	}

	//	ヘルプ
	DrawFormatString(840, 20, 0x000000, "移動　　：WASD");
	DrawFormatString(840, 40, 0x000000, "カメラ　：矢印キー");
	DrawFormatString(840, 60, 0x000000, "ダッシュ：右Shift");
	DrawFormatString(840, 80, 0x000000, "ジャンプ：＼(バクスラ)");
}
