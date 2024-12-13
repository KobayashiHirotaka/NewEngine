/**
 * @file Mesh.cpp
 * @brief メッシュの初期化、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Mesh.h"

void Mesh::Initialize(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices)
{
	//DirectXCoreのインスタンスの取得
	dxCore_ = DirectXCore::GetInstance();

	//頂点データをセット
	vertices_ = vertices;

	//インデックスデータをセット
	indices_ = indices;

	//頂点バッファの作成
	CreateVertexBuffer();

	//インデックスバッファの作成
	CreateIndexBuffer();
}

void Mesh::Draw()
{
	//インスタンシング描画
	dxCore_->GetCommandList()->DrawIndexedInstanced(UINT(indices_.size()), kInstanceCount_, 0, 0, 0);
}

void Mesh::SetGraphicsCommand(D3D12_VERTEX_BUFFER_VIEW influenceBufferView)
{
	//頂点バッファビューをセット
	D3D12_VERTEX_BUFFER_VIEW vbvs[kNumViews_] = { vertexBufferView_,influenceBufferView };

	//頂点バッファとインデックスバッファをセット
	dxCore_->GetCommandList()->IASetVertexBuffers(0, kNumViews_, vbvs);
	dxCore_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	//プリミティブ形状を三角形に設定
	dxCore_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::CreateVertexBuffer()
{
	//頂点バッファを作成
	vertexBuffer_ = dxCore_->CreateBufferResource(sizeof(VertexData) * vertices_.size());

	//バッファビューの設定
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//データを転送
	VertexData* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexData) * vertices_.size());
	vertexBuffer_->Unmap(0, nullptr);
}

void Mesh::CreateIndexBuffer()
{
	//インデックスバッファを作成
	indexBuffer_ = dxCore_->CreateBufferResource(sizeof(uint32_t) * indices_.size());

	//バッファビューの設定
	indexBufferView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * indices_.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//データを転送
	uint32_t* indexData;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());
	indexBuffer_->Unmap(0, nullptr);
}