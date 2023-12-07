#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/Utility/Structs/VertexData.h"
#include "Engine/Utility/Structs/Material.h"
#include <wrl.h>

class Triangle
{
public:
	void Initialize();
	void Draw(const WorldTransform& transform, const Camera& camera, const uint32_t textureHandle);
	void ImGui(const char* Title);

private:
	void CreateVertexBufferView();

private:
	DirectXCore* dxCore_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	Transform tranformTriAngle_
	{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	Transform uvTranformTriAngle_
	{
	    {1.0f,1.0f,1.0f},
	    {0.0f,0.0f,0.0f},
	    {0.0f,0.0f,0.0f}
	};

	//バーテックスリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>vertexResource_ = nullptr;

	//頂点データ
	VertexData* vertexData_ = nullptr;

	//バーテックスバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	//色データ
	Material* materialData_ = nullptr;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
};

