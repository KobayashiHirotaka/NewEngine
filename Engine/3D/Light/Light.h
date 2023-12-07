#pragma once
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Engine/Utility/Math/MyMath.h"
#include <d3d12.h>
#include <wrl.h>

enum Lighting 
{
	checkLighting = false,
	harfLambert = 1,
	lambert = 2,
};

class Light
{
	struct DirectionalLight
	{
		Vector4 color;
		Vector3 direction;
		float intensity;
	};

public:
	static Light* GetInstance();

	//Light();
	//~Light();

	void Initialize();

	void ImGui(const char* Title);

	ID3D12Resource* GetDirectionalLight()const { return directionalLightResource_.Get(); }

private:
	DirectXCore* dxCore_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource>directionalLightResource_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;
};

