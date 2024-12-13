/**
 * @file Mesh.h
 * @brief メッシュの初期化、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"
#include <vector>

class Mesh
{
public:
	/// <summary>初期化</summary>
	void Initialize(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices);

	/// <summary>描画</summary>
	void Draw();

	/// <summary>グラフィックスコマンドをセット</summary>
	void SetGraphicsCommand(D3D12_VERTEX_BUFFER_VIEW influenceBufferView);

private:
	/// <summary>頂点バッファの作成</summary>
	void CreateVertexBuffer();

	/// <summary>インデックスバッファの作成</summary>
	void CreateIndexBuffer();

private:
	//DirectXCoreのポインタ
	DirectXCore* dxCore_ = nullptr;

	//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	std::vector<VertexData> vertices_{};

	//インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	std::vector<uint32_t> indices_{};

	//インスタンスカウント
	const int  kInstanceCount_ = 1;

	//ビューの数
	static const int kNumViews_ = 2;
};