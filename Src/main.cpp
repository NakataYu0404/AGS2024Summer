#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <memory>
#include <DxLib.h>
#include "Application.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//	WinMain関数
//	---------------------------------
int WINAPI WinMain(
	_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

	//	メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//	�@生ポインタ
	//	int *test = new int();
	//	delete test;
	
	//	�Aユニークポインタ
	std::unique_ptr<int> testI = std::make_unique<int>();
	std::unique_ptr<std::string> testS = std::make_unique<std::string>();
	

	//	インスタンスの生成
	Application::CreateInstance();

	//	インスタンスの取得
	Application& instance = Application::GetInstance();

	if (instance.IsInitFail())
	{
		//	初期化失敗
		return -1;
	}

	//	実行
	instance.Run();

	//	解放
	instance.Destroy();

	return 0;

}
