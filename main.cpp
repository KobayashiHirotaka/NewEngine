#include "Project/Game/Scenes/Manager/SceneManager.h"

struct D3DResourceLeakCheker
{
	~D3DResourceLeakCheker()
	{
		Microsoft::WRL::ComPtr<IDXGIDebug1>debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			debug->Release();
		}
	}
};

int IScene::sceneNum;

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	SceneManager* sceneManager = new SceneManager();
	sceneManager->Run();

	//D3DResourceLeakCheker leakChecker;

	return 0;
}