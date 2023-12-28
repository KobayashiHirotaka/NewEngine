#include "Engine/Base/WindowsApp/WindowsApp.h"
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/Base/D3DResourceLeakChecker.h"
//#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Project/Game/Scenes/IScene.h"
#include <memory>

class SceneManager 
{
public:
	SceneManager();

	~SceneManager();

	void ChangeScene(IScene* newScene);

	void Run();

private:
	static D3DResourceLeakChecker leakCheck;

	WindowsApp* win_ = nullptr;

	DirectXCore* dxCore_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	ImGuiManager* imguiManager_ = nullptr;

	/*Audio* audio_ = nullptr;*/

	Input* input_ = nullptr;

	PostProcess* postProcess_ = nullptr;

	IScene* currentScene_;
};