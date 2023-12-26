#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"
#include <vector>

class Mesh
{
public:
	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	void Initialize(const std::vector<VertexData>& vertices);

	void SetGraphicsCommand();

	void Draw();

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	
	std::vector<VertexData> vertices_{};
};