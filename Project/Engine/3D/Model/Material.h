#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"

class Material
{
public:
	struct ConstBufferDataMaterial
	{
		Vector4 color;
		Matrix4x4 uvTransform;
		float shininess;
	};

	void Initialize();

	void Update();

	void SetGraphicsCommand(UINT rootParameterIndex);

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
	const Vector2& GetScale() const { return scale_; };
	void SetScale(const Vector2& scale) { scale_ = scale; };

private:
	DirectXCore* dxCore_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector2 translation_ = { 0.0f,0.0f };

	float rotation_ = 0.0f;

	Vector2 scale_ = { 1.0f,1.0f };

	float shininess_ = 40.8f;
};