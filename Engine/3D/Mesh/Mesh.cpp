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
	//VBVを設定
	DirectXCore::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとは別。同じものを設定すると考えておけば良い
	DirectXCore::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::Draw() 
{
	//描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	DirectXCore::GetInstance()->GetCommandList()->DrawInstanced(UINT(vertices_.size()), 1, 0, 0);
}
