#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"
#include <vector>

class Mesh
{
public:
	void Initialize(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices);

	void Draw();

	void SetGraphicsCommand();

private:
	DirectXCore* dxCore_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	
	std::vector<VertexData> vertices_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_ = nullptr;

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	std::vector<uint32_t> indices_{};
};