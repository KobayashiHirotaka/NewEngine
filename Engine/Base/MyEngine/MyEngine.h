#pragma once
#include "Engine/Base/WindowsApp/WindowsApp.h"
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include <fstream>
#include <numbers>

class MyEngine
{
public:
	static MyEngine* GetInstance();

	//MyEngine();
	//~MyEngine();

	void Initialize();

	void ImGui();

private:
	WindowsApp* win_ = nullptr;

	DirectXCore* dxCore_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	HRESULT hr_;
};
