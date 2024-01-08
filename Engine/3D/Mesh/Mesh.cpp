#include "Mesh.h"

void Mesh::Initialize(const std::vector<VertexData>& vertices)
{
	vertices_ = vertices;

	vertexBuffer_ = DirectXCore::GetInstance()->CreateBufferResource(sizeof(VertexData) * vertices_.size());

	//頂点バッファビューを作成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点バッファにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexData) * vertices_.size());
	vertexBuffer_->Unmap(0, nullptr);
}

void Mesh::SetGraphicsCommand() 
{
	DirectXCore::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	DirectXCore::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::Draw() 
{
	DirectXCore::GetInstance()->GetCommandList()->DrawInstanced(UINT(vertices_.size()), 1, 0, 0);
}
