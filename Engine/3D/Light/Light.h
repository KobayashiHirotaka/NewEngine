#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"

class Light
{
public:
	enum class LightingMethod 
	{
		LambertianReflectance,
		HalfLambert
	};

	struct ConstBufferDataDirectionalLight
	{
		int32_t enableLighting;

		LightingMethod lightingMethod;
	
		float padding[2];
	
		Vector4 color;
	
		Vector3 direction;
	
		float intensity;
	};

	void Initialize();

	void Update();

	void SetGraphicsCommand(UINT rootParameterIndex);

	const int32_t& GetEnableLighting() const { return enableLighting_; };

	void SetEnableLighting(const int32_t& enableLighting) { enableLighting_ = enableLighting; };

	const int32_t& GetLightingMethod() const { return int32_t(lightingMethod_); };

	void SetLightingMethod(const int32_t& lightingMethod) { lightingMethod_ = LightingMethod(lightingMethod); };

	const Vector4& GetColor() const { return color_; };

	void SetColor(const Vector4& color) { color_ = color; };

	const Vector3& GetDirection() const { return direction_; };

	void SetDirection(const Vector3& direction) { direction_ = direction; };

	const float& GetIntensity() const { return intensity_; };

	void SetIntensity(const float& intensity) { intensity_ = intensity; };

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> lightingResource_ = nullptr;
	
	int32_t enableLighting_ = false;

	LightingMethod lightingMethod_ = LightingMethod::HalfLambert;
	
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector3 direction_ = { 0.0f,-1.0f,0.0f };
	
	float intensity_ = 1.0f;
};