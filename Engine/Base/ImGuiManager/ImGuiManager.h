#pragma once
#include "Engine/Base/WindowsApp/WindowsApp.h"
#include "Engine/Base/DirectXCore/DirectXCore.h"

class ImGuiManager
{
public:
	static ImGuiManager* GetInstance();

	//ImGuiManager();
	//~ImGuiManager();

	void Initialize(WindowsApp* win, DirectXCore* dxCore);

	void BeginFlame();

	void EndFlame();
};

