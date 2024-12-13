/**
 * @file TextureManager.h
 * @brief テクスチャの管理(読み込みやSRVの作成など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/externals/DirectXTex/DirectXTex.h"
#include "Engine/externals/DirectXTex/d3dx12.h"
#include "Engine/Utility/ConvertString/ConvertString.h"
#include <array>
#include <cassert>
#include <thread>
#include <vector>

class TextureManager
{
public:
	//最大ディスクリプタ数
	static const size_t kMaxDescriptors = 256;

	//SRVディスクリプタサイズ
	static uint32_t sDescriptorSizeSRV;

	struct Texture
	{
		//リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = nullptr;

		//テクスチャハンドル
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
		uint32_t textureHandle = 0;

		//名前
		std::string name{};
	};

	/// <summary>インスタンスの取得</summary>
	static TextureManager* GetInstance();

	/// <summary>インスタンスの削除</summary>
	static void DeleteInstance();

	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>テクスチャの読み込み</summary>
	static uint32_t LoadTexture(const std::string& filePath);

	/// <summary>ディスクリプタヒープの設定</summary>
	void SetGraphicsDescriptorHeap();

	/// <summary>ディスクリプタテーブルの設定</summary>
	void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, uint32_t textureHandle);

	/// <summary>インスタンシング用SRVの作成</summary>
	uint32_t CreateInstancingSRV(const Microsoft::WRL::ComPtr<ID3D12Resource>& instancingResource, uint32_t kNumInstance, size_t size);

	/// <summary>リソースディスクの取得</summary>
	const D3D12_RESOURCE_DESC GetResourceDesc(uint32_t textureHandle);

	/// <summary>ディスクリプタハンドルの取得</summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, const uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, const uint32_t descriptorSize, uint32_t index);

private:
	//シングルトン
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	//テクスチャ読み込み
	uint32_t LoadInternal(const std::string& filePath);

	//画像を開く
	DirectX::ScratchImage OpenImage(const std::string& filePath);

	//テクスチャリソースを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	//テクスチャデータをアップロード
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

private:
	//TextureManagerのインスタンス
	static TextureManager* sInstance_;

	//DirectX
	DirectXCore* dxCore_ = nullptr;
	ID3D12Device* device_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//SRV用ディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;

	//テクスチャ
	std::array<Texture, kMaxDescriptors> textures_{};

	//テクスチャハンドル
	uint32_t textureHandle_ = 0xFFFFFFFF;
};
