#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Light/Light.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/Utility/Structs/VertexData.h"
#include "Engine/Utility/Structs/Material.h"
#include <wrl.h>
#include <numbers>

class Sphere
{
public:
	void Initialize();
	void Draw(const WorldTransform& transform, const Camera& camera, const uint32_t& textureHandle);
	void ImGui(const char* Title);

private:
	void CreateVertexBufferViewSphere();
	void CreateIndexBufferViewSphere();

private:
	DirectXCore* dxCore_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	Light* light_ = nullptr;

	const int kSubdivision_ = 16;

	int32_t lightFlag = Lighting::checkLighting;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSphere_ = nullptr;

	VertexData* vertexDataSphere_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSphere_ = nullptr;

	Material* materialDataSphere_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSphere_ = nullptr;

	uint32_t* indexDataSphere_;

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSphere_{};

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
};
