#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/externals/DirectXTex/DirectXTex.h"
#include <wrl.h>
#include <array>

class TextureManager
{
public:

	struct Texture
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;

		CD3DX12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
		CD3DX12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

		std::string name;
	};

	static TextureManager* GetInstance();

	void Initialize(DirectXCore* dxCore);

	uint32_t LoadTexture(const std::string& filePath);

	const CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t textureHandle);

	const D3D12_RESOURCE_DESC GetResourceDesc(uint32_t textureHandle);

private:
	static const size_t kMaxTexture = 256;

	HRESULT hr_;

	DirectXCore* dxCore_ = nullptr;

	bool CheckTextureIndex[kMaxTexture];

	std::array<Texture, kMaxTexture> textures_;

	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource_[kMaxTexture];

	DirectX::ScratchImage OpenImage(const std::string& filePath);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	//DescriptorSizeを取得しておく
	uint32_t descriptorSizeSRV_;
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;
};

