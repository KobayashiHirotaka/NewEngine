/**
 * @file Light.h
 * @brief ライトの初期化、更新、ライティングの設定などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Engine/3D/Light/Types.h"

class Light
{
public:
	struct ConstBuffDataLight
	{
		//ライティングするか
		int32_t enableLighting;

		//使用するライティングのタイプ
		LightingType lightingType;

		//モデルのタイプ
		ModelType modelType;

		//パディング
		float padding;

		//ライティングの色 
		Vector4 color;

		//ライティングの方向
		Vector3 direction;

		//ライティングの強度
		float intensity;
	};

	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>更新</summary>
	void Update();

	/// <summary>グラフィックスコマンドを設定</summary>
	void SetGraphicsCommand(UINT rootParameterIndex);

	/// <summary>ImGui</summary>
	void ImGui();

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

	//Intensity
	const float& GetIntensity() const { return intensity_; };
	void SetIntensity(const float& intensity) { intensity_ = intensity; };

private:
	//DirectXCoreのポインタ
	DirectXCore* dxCore_ = nullptr;

	//ライティングリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> lightingResource_ = nullptr;

	//ライティングするか
	int32_t enableLighting_ = true;

	//ライティングタイプ
	LightingType lightingType_ = LightingType::HalfLambert;

	//モデルタイプ
	ModelType modelType_ = ModelType::PhongReflectionModel;

	//ライティングの色
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	//ライティングの方向
	Vector3 direction_ = { -1.0f, -5.5f, 0.67f };

	//ライティングの強度
	float intensity_ = 1.0f;
};