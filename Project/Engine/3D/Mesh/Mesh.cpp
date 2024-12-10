/**
 * @file Mesh.cpp
 * @brief メッシュの初期化、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Mesh.h"

void Mesh::Initialize(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices)
{
	dxCore_ = DirectXCore::GetInstance();

	vertices_ = vertices;

	indices_ = indices;

	CreateVertexBuffer();

	CreateIndexBuffer();
}

void Mesh::Draw()
{
	//sDxCore_->GetCommandList()->DrawInstanced(UINT(vertices_.size()), 1, 0, 0);
	dxCore_->GetCommandList()->DrawIndexedInstanced(UINT(indices_.size()), 1, 0, 0, 0);
}

void Mesh::SetGraphicsCommand(D3D12_VERTEX_BUFFER_VIEW influenceBufferView)
{
	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = { vertexBufferView_,influenceBufferView };

	dxCore_->GetCommandList()->IASetVertexBuffers(0, 2, vbvs);
	dxCore_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	dxCore_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::CreateVertexBuffer()
{
	vertexBuffer_ = dxCore_->CreateBufferResource(sizeof(VertexData) * vertices_.size());

	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	VertexData* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexData) * vertices_.size());
	vertexBuffer_->Unmap(0, nullptr);
}

void Mesh::CreateIndexBuffer()
{
	indexBuffer_ = dxCore_->CreateBufferResource(sizeof(uint32_t) * indices_.size());

	indexBufferView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * indices_.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexData;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());
	indexBuffer_->Unmap(0, nullptr);
}