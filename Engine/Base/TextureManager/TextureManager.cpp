#include "TextureManager.h"

TextureManager* TextureManager::GetInstance()
{
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize(DirectXCore* dxCore)
{
	dxCore_ = dxCore;
	descriptorSizeSRV_ = dxCore_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV_ = dxCore_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV_ = dxCore_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

uint32_t TextureManager::LoadTexture(const std::string& filePath)
{
	uint32_t spriteIndex = kMaxTexture + 1;

	for (int i = 0; i < kMaxTexture; ++i)
	{
		if (CheckTextureIndex[i] == false)
		{
			spriteIndex = i;
			CheckTextureIndex[i] = true;
			break;
		}
	}

	if (spriteIndex < 0)
	{
		assert(false);
	}

	if (kMaxTexture < spriteIndex)
	{
		assert(false);
	}

	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = OpenImage(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	textures_.at(spriteIndex).textureResource = CreateTextureResource(dxCore_->GetDevice(), metadata);
	intermediateResource_[spriteIndex] = UploadTextureData(textures_.at(spriteIndex).textureResource, mipImages);

	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	textures_.at(spriteIndex).textureSrvHandleCPU = GetCPUDescriptorHandle(dxCore_->GetSrvDescriptorHeap(), descriptorSizeSRV_, spriteIndex);
	textures_.at(spriteIndex).textureSrvHandleGPU = GetGPUDescriptorHandle(dxCore_->GetSrvDescriptorHeap(), descriptorSizeSRV_, spriteIndex);
	//先頭はImGuiが使っているので次のを使う
	textures_.at(spriteIndex).textureSrvHandleCPU.ptr += dxCore_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textures_.at(spriteIndex).textureSrvHandleGPU.ptr += dxCore_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//SRVの作成
	dxCore_->GetDevice()->CreateShaderResourceView(textures_.at(spriteIndex).textureResource.Get(), &srvDesc, textures_.at(spriteIndex).textureSrvHandleCPU);

	return spriteIndex;
}

const CD3DX12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUHandle(uint32_t textureHandle)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle = textures_.at(textureHandle).textureSrvHandleGPU;
	return GPUHandle;
}

DirectX::ScratchImage TextureManager::OpenImage(const std::string& filePath)
{
	//テクスチャファイルを読み込みプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	hr_ = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr_));
	//ミップマップの作成
	DirectX::ScratchImage mipImage{};
	hr_ = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImage);

	return mipImage;
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata)
{
	//metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);
	//利用するheapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//Resourceの生成
	ID3D12Resource* resource = nullptr;
	hr_ = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);

	assert(SUCCEEDED(hr_));

	return resource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages)
{
	std::vector<D3D12_SUBRESOURCE_DATA>subresources;
	DirectX::PrepareUpload(dxCore_->GetDevice(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = dxCore_->CreateBufferResource(intermediateSize);
	UpdateSubresources(dxCore_->GetcommandList(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	//Tetureへの転送後は利用できるようにD3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	dxCore_->GetcommandList()->ResourceBarrier(1, &barrier);

	return intermediateResource;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}