#pragma once
#include "Engine/Base/WindowsApp/WindowsApp.h"
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/Base/D3DResourceLeakChecker.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Engine/3D/Particle/Random.h"
#include "Engine/Utility/GlobalVariables.h"
#include "Engine/Framework/SceneManager.h"

class EngineCore
{
public:
	virtual ~EngineCore() = default;

	virtual void Initialize();

	virtual void Finalize();

	virtual void Update();

	virtual void Draw() = 0;

	virtual bool IsEndRequst();

	void Run();

private:
	static D3DResourceLeakChecker leakCheck;

	WindowsApp* win_ = nullptr;

	DirectXCore* dxCore_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	ImGuiManager* imguiManager_ = nullptr;

	Audio* audio_ = nullptr;

	Input* input_ = nullptr;

	PostProcess* postProcess_ = nullptr;

	SceneManager* sceneManager_ = nullptr;

	bool endRequst_ = false;
};

