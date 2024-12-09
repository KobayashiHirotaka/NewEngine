/**
 * @file SpotLight.h
 * @brief スポットライトの初期化、更新、ライティングの設定などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Engine/3D/Light/Types.h"
#include <numbers>

class SpotLight
{
public:
	struct ConstBuffDataSpotLight
	{
		int32_t enableLighting;

		LightingType lightingType;

		ModelType modelType;

		float padding;

		Vector4 color;

		Vector3 direction;

		float distance;

		Vector3 position;

		float intensity;

		float decay;

		float cosAngle;

		float cosFalloffStart;
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

	//ModelType
	const int32_t& GetModelType() const { return int32_t(modelType_); };
	void SetModelType(const int32_t& modelType) { modelType_ = ModelType(modelType); };

	//Color
	const Vector4& GetColor() const { return color_; };
	void SetColor(const Vector4& color) { color_ = color; };

	//Direction
	Vector3& GetDirection() { return direction_; };
	void SetDirection(const Vector3& direction) { direction_ = direction; };

	//Distance
	const float& GetDistance() const { return distance_; };
	void SetDistance(const float& distance) { distance_ = distance; };

	//Position
	Vector3& GetPosition() { return position_; };
	void SetPosition(const Vector3& position) { position_ = position; };

	//Intensity
	const float& GetIntensity() const { return intensity_; };
	void SetIntensity(const float& intensity) { intensity_ = intensity; };

	//cosAngle
	const float& GetCosAngle() const { return cosAngle_; };
	void SetCosAngle(const float& cosAngle) { cosAngle_ = cosAngle; };

	//cosFalloffStart
	const float& GetCosFalloffStart() const { return cosFalloffStart_; };
	void SetCosFalloffStart(const float& cosFalloffStart) { cosFalloffStart_ = cosFalloffStart; };

private:
	DirectXCore* dxCore_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> lightingResource_ = nullptr;

	int32_t enableLighting_ = false;

	LightingType lightingType_ = LightingType::HalfLambert;

	ModelType modelType_ = ModelType::PhongReflectionModel;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector3 direction_ = Normalize({ -1.0f,-1.0f,0.0f });

	float distance_ = 7.0f;

	Vector3 position_ = { 2.0f,1.25f,0.0f };

	float intensity_ = 4.0f;

	float decay_ = 2.0f;

	float cosAngle_ = std::cos(std::numbers::pi_v<float> / 3.0f);

	float cosFalloffStart_ = 1.0f;
};


