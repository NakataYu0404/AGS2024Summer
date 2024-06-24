#include <memory>
#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/AnimationController.h"
#include "../Object/SkyDome.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
{
	imgPush_ = -1;
	imgTitle_ = -1;
	skyDome_ = nullptr;
	animationController_ = nullptr;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{

	//	画像読み込み
	imgTitle_ = resMng_.Load(ResourceManager::SRC::IMG_TITLE).handleId_;
	imgPush_ = resMng_.Load(ResourceManager::SRC::IMG_PUSHSPACE).handleId_;

	//	背景
	spaceDomeTran_.pos = AsoUtility::VECTOR_ZERO;
	skyDome_ = std::make_unique<SkyDome>();
	skyDome_->Init();

	float size;

	//	メイン惑星
	planet_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MDL_FALLPLANET));
	planet_.pos = AsoUtility::VECTOR_ZERO;
	planet_.scl = AsoUtility::VECTOR_ONE;
	planet_.Update();

	//	回転する惑星
	movePlanet_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MDL_LASTPLANET));
	movePlanet_.pos = { -250.0f, -100.0f, -100.0f };
	size = 0.7f;
	movePlanet_.scl = { size, size, size };
	movePlanet_.quaRotLocal = Quaternion::Euler(
		AsoUtility::Deg2RadF(90.0f), 0.0f, 0.0f);
	movePlanet_.Update();

	//	キャラ
	charactor_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MDL_PLAYER));
	charactor_.pos = { -250.0f, -32.0f, -105.0f };
	size = 0.4f;
	charactor_.scl = { size, size, size };
	charactor_.quaRot = Quaternion::Euler(
		0.0f, AsoUtility::Deg2RadF(90.0f), 0.0f);
	charactor_.Update();

	//	アニメーションの設定
	std::string path = Application::PATH_MODEL + "Player/Anim/";
	animationController_ = std::make_unique<AnimationController>(charactor_.modelId);
	animationController_->Add(0, path + "Run.mv1", 20.0f);
	animationController_->Play(0);

	//	定点カメラ
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);

	ShaderVertHdl_ = LoadVertexShader("./Data/Shader/LightVertexShader.cso");
	ShaderPixHdl_ = LoadPixelShader("./Data/Shader/LighPixelShader.cso");
	cnt_ = 0.0f;
	lightBuf = CreateShaderConstantBuffer(sizeof(float) * 4);
}

void TitleScene::Update(void)
{

	//	シーン遷移
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}

	//	惑星の回転
	movePlanet_.quaRot = movePlanet_.quaRot.Mult(
		Quaternion::Euler(0.0f, 0.0f, AsoUtility::Deg2RadF(-1.0f)));
	movePlanet_.Update();

	//	キャラアニメーション
	animationController_->Update();

	skyDome_->Update();

	cnt_++;
}

void TitleScene::Draw(void)
{
	skyDome_->Draw();

	MV1SetUseOrigShader(true);
	SetUseVertexShader(ShaderVertHdl_);
	SetUsePixelShader(ShaderPixHdl_);
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	ChangeLightTypeDir({ sinf(AsoUtility::Deg2RadF(cnt_)), 0.0f,sinf(AsoUtility::Deg2RadF(cnt_)+ AsoUtility::Deg2RadF(90.0f))});
	lightDir.x = sinf(AsoUtility::Deg2RadF(cnt_));
	lightDir.y = 0.0f;
	lightDir.z = sinf(AsoUtility::Deg2RadF(cnt_) + AsoUtility::Deg2RadF(90.0f));
	lightDir.w = 0.0f;

	lightDir.x = VNorm({ lightDir.x ,lightDir.y,lightDir.z }).x;
	lightDir.y = VNorm({ lightDir.x ,lightDir.y,lightDir.z }).y;
	lightDir.z = VNorm({ lightDir.x ,lightDir.y,lightDir.z }).z;

	FLOAT4* tmpLightBuf = (FLOAT4*)GetBufferShaderConstantBuffer(lightBuf);

	tmpLightBuf->x = lightDir.x;
	tmpLightBuf->y = lightDir.y;
	tmpLightBuf->z = lightDir.z;
	tmpLightBuf->w = lightDir.w;

	UpdateShaderConstantBuffer(lightBuf);
	SetShaderConstantBuffer(lightBuf, DX_SHADERTYPE_VERTEX, 4);
	SetShaderConstantBuffer(lightBuf, DX_SHADERTYPE_PIXEL, 4);


	MV1DrawModel(planet_.modelId);
	MV1DrawModel(movePlanet_.modelId);
	MV1SetUseOrigShader(false);

	MV1DrawModel(charactor_.modelId);

	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 250, 1.0, 0.0, imgTitle_, true);
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 500, 1.0, 0.0, imgPush_, true);

}
