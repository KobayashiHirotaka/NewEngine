#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/Utility/Structs/VertexData.h"
#include "Engine/Utility/Structs/Material.h"
#include <wrl.h>

class Sprite
{
public:
	void Initialize(const Vector4& LeftTop, const Vector4& LeftBottom, const Vector4& RightTop, const Vector4& RightBottom, const uint32_t textureHandle);
	void Update(const Vector4& LeftTop, const Vector4& LeftBottom, const Vector4& RightTop, const Vector4& RightBottom);
	void Draw(const WorldTransform& transform);
	void ImGui(const char* Title);

private:
	void CreateVertexBufferViewSprite();
	void CreateIndexBufferViewSprite();
	void AdjustTextureSize();

private:
	DirectXCore* dxCore_ = nullptr;

	TextureManager* textureManager_ = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite_ = nullptr;

	VertexData* vertexDataSprite_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite_ = nullptr;

	Material* materialDataSprite_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite_ = nullptr;

	uint32_t* indexDataSprite_;

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite_{};

	Transform transformSprite_
	{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	Transform uvTransformSprite_
	{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	Vector2 textureLeftTop_ = { 0.0f,0.0f };

	Vector2 textureSize_ = { 100.0f,100.0f };

	Camera camera_;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	uint32_t textureHandle_;

	D3D12_RESOURCE_DESC resDesc_;
};

