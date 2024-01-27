#include "Mesh.h"

void Mesh::Initialize(const std::vector<VertexData>& vertices)
{
	dxCore_ = DirectXCore::GetInstance();

	vertices_ = vertices;

	vertexBuffer_ = dxCore_->CreateBufferResource(sizeof(VertexData) * vertices_.size());

	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	VertexData* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexData) * vertices_.size());
	vertexBuffer_->Unmap(0, nullptr);
}

void Mesh::Draw() 
{
	dxCore_->GetCommandList()->DrawInstanced(UINT(vertices_.size()), 1, 0, 0);
}

void Mesh::SetGraphicsCommand()
{
	dxCore_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	dxCore_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
