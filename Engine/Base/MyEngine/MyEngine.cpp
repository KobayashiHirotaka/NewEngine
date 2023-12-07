#include "MyEngine.h"
#include "Project/Game/Scenes/Manager/SceneManager.h"

MyEngine* MyEngine::GetInstance()
{
	static MyEngine instance;
	return &instance;
}

void MyEngine::Initialize()
{
	win_ = WindowsApp::GetInstance();
	win_->Initialize();

	dxCore_ = DirectXCore::GetInstance();
	dxCore_->Initialize(win_);

	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize(dxCore_);
}

void MyEngine::ImGui()
{
	ImGui::ShowDemoWindow();
}

