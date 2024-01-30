#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"

enum class LightingType
{
	LambertianReflectance,
	HalfLambert,
};

class Light
{
public:
	struct ConstBuffDataLight
	{
		int32_t enableLighting;

		LightingType lightingType;
	
		float padding[2];
	
		Vector4 color;
	
		Vector3 direction;
	
		float intensity;
	};

	void Initialize();

	void Update();

	void SetGraphicsCommand(UINT rootParameterIndex);

	void ImGui(const char* Title);

	//EnableLighting
	const int32_t& GetEnableLighting() const { return enableLighting_; };
	void SetEnableLighting(const int32_t& enableLighting) { enableLighting_ = enableLighting; };

	//LightingType
	const int32_t& GetLightingType() const { return int32_t(lightingType_); };
	void SetLightingType(const int32_t& lightingType) { lightingType_ = LightingType(lightingType); };

	//Color
	const Vector4& GetColor() const { return color_; };
	void SetColor(const Vector4& color) { color_ = color; };

	//Direction
	Vector3& GetDirection(){ return direction_; };
	void SetDirection(const Vector3& direction) { direction_ = direction; };

	//Intensity
	const float& GetIntensity() const { return intensity_; };
	void SetIntensity(const float& intensity) { intensity_ = intensity; };

private:
	DirectXCore* dxCore_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> lightingResource_ = nullptr;
	
	int32_t enableLighting_ = true;

	LightingType lightingType_ = LightingType::HalfLambert;
	
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector3 direction_ = { 0.0f,-1.0f,0.0f };
	
	float intensity_ = 1.0f;
};