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
	static const size_t kMaxDescriptors = 256;

	static uint32_t descriptorSizeSRV;

	struct Texture 
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = nullptr;
	
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	
		std::string name{};

		uint32_t textureHandle = 0;
	};

	static TextureManager* GetInstance();

	static void DeleteInstance();

	void Initialize();

	static uint32_t LoadTexture(const std::string& filePath);

	void SetGraphicsDescriptorHeap();

	void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, uint32_t textureHandle);

	uint32_t CreateInstancingSRV(const Microsoft::WRL::ComPtr<ID3D12Resource>& instancingResource, uint32_t kNumInstance, size_t size);

	const D3D12_RESOURCE_DESC GetResourceDesc(uint32_t textureHandle);

private:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	uint32_t LoadInternal(const std::string& filePath);

	DirectX::ScratchImage OpenImage(const std::string& filePath);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, const uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, const uint32_t descriptorSize, uint32_t index);

private:
	static TextureManager* instance_;

	DirectXCore* dxCore_ = nullptr;

	ID3D12Device* device_ = nullptr;

	ID3D12GraphicsCommandList* commandList_ = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
	
	std::array<Texture, kMaxDescriptors> textures_{};

	uint32_t textureHandle_ = -1;
};
