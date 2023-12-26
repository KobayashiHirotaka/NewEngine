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
	};

	void Initialize();

	void Update();

	void SetGraphicsCommand(UINT rootParameterIndex);

	const Vector4& GetColor() const { return color_; };

	void SetColor(const Vector4& color) { color_ = color; };

	const Vector2& GetTranslation() const { return translation_; };

	void SetTranslation(const Vector2& translation) { translation_ = translation; };

	const float& GetRotation() const { return rotation_; };

	void SetRotation(const float& rotation) { rotation_ = rotation; };

	const Vector2& GetScale() const { return scale_; };

	void SetScale(const Vector2& scale) { scale_ = scale; };

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector2 translation_{ 0.0f,0.0f };

	float rotation_ = 0.0f;
	
	Vector2 scale_{ 1.0f,1.0f };
};