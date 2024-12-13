/**
 * @file Material.h
 * @brief マテリアルの初期化、更新などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"

class Material
{
public:
	struct ConstBufferDataMaterial
	{
		//色
		Vector4 color;

		//UV変換行列
		Matrix4x4 uvTransform;

		//光沢度
		float shininess;

		//環境光
		float environmentCofficient;
	};

	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>更新</summary>
	void Update();

	/// <summary>グラフィックスコマンドの設定</summary>
	void SetGraphicsCommand(UINT rootParameterIndex);

	/// <summary>ImGui</summary>
	void ImGui();

	//Getter・Setter
	//Color
	const Vector4& GetColor() const { return color_; };
	void SetColor(const Vector4& color) { color_ = color; };

	//Translation
	const Vector2& GetTranslation() const { return translation_; };
	void SetTranslation(const Vector2& translation) { translation_ = translation; };

	//Rotation
	const float& GetRotation() const { return rotation_; };
	void SetRotation(const float& rotation) { rotation_ = rotation; };

	//Scale
	const Vector3& GetScale() const { return scale_; };
	void SetScale(const Vector3& scale) { scale_ = scale; };

	//EnvironmentCofficient
	const float& GetEnvironmentCofficient() const { return environmentCofficient_; };
	void SetEnvironmentCofficient(const float& environmentCofficient) { environmentCofficient_ = environmentCofficient; };

private:
	//DirectXCoreのポインタ
	DirectXCore* dxCore_ = nullptr;

	//マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	//色
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	//位置の平行移動量
	Vector2 translation_ = { 0.0f,0.0f };

	//回転
	float rotation_ = 0.0f;

	//スケール
	Vector3 scale_ = { 1.0f,1.0f,1.0f };

	//光沢度
	float shininess_ = 40.8f;

	//環境光
	float environmentCofficient_ = 0.0f;
};