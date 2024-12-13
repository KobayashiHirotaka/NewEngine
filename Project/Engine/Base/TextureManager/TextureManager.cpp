/**
 * @file TextureManager.cpp
 * @brief テクスチャの管理(読み込みやSRVの作成など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "TextureManager.h"

uint32_t TextureManager::sDescriptorSizeSRV = 0;
TextureManager* TextureManager::sInstance_ = nullptr;

TextureManager* TextureManager::GetInstance()
{
	//インスタンスを生成
	if (sInstance_ == nullptr)
	{
		sInstance_ = new TextureManager();
	}
	return sInstance_;
}

void TextureManager::DeleteInstance()
{
	//インスタンスを削除
	if (sInstance_ != nullptr)
	{
		delete sInstance_;
		sInstance_ = nullptr;
	}
}

void TextureManager::Initialize()
{
	//DirectXCoreのインスタンスの取得
	dxCore_ = DirectXCore::GetInstance();

	//デバイスの取得
	device_ = dxCore_->GetDevice();

	//コマンドリストの取得
	commandList_ = dxCore_->GetCommandList();

	//SRVディスクリプタインクリメントサイズを取得
	sDescriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//SRVディスクリプタヒープの作成
	srvDescriptorHeap_ = dxCore_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxDescriptors, true);

	//初期テクスチャの読み込み
	LoadInternal("resource/images/white.png");
}

uint32_t TextureManager::LoadTexture(const std::string& filePath)
{
	//テクスチャの読み込み
	uint32_t textureHandle = TextureManager::GetInstance()->LoadInternal(filePath);

	return textureHandle;
}

void TextureManager::SetGraphicsDescriptorHeap()
{
	//コマンドリストにディスクリプタヒープを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);
}

void TextureManager::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, uint32_t textureHandle)
{
	//コマンドリストにディスクリプタテーブルを設定
	commandList_->SetGraphicsRootDescriptorTable(rootParameterIndex, textures_[textureHandle].textureSrvHandleGPU);
}

uint32_t TextureManager::CreateInstancingSRV(const Microsoft::WRL::ComPtr<ID3D12Resource>& instancingResource, uint32_t kNumInstance, size_t size)
{
	textureHandle_++;

	//インスタンシング用のSRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumInstance;
	instancingSrvDesc.Buffer.StructureByteStride = UINT(size);

	//SRVのディスクリプタを設定
	textures_[textureHandle_].textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap_.Get(), sDescriptorSizeSRV, textureHandle_);
	textures_[textureHandle_].textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap_.Get(), sDescriptorSizeSRV, textureHandle_);

	//SRVをリソースに設定
	device_->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, textures_[textureHandle_].textureSrvHandleCPU);
	return textureHandle_;
}

uint32_t TextureManager::LoadInternal(const std::string& filePath)
{
	//同じテクスチャがすでに読み込まれていれば、そのハンドルを返す
	for (int i = 0; i < kMaxDescriptors; i++)
	{
		if (textures_[i].name == filePath)
		{
			return textures_[i].textureHandle;
		}
	}

	textureHandle_++;

	//最大ハンドル数を超えた場合
	if (textureHandle_ >= kMaxDescriptors)
	{
		assert(0);
	}

	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = OpenImage(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	textures_[textureHandle_].resource = CreateTextureResource(metadata);
	textures_[textureHandle_].intermediateResource = UploadTextureData(textures_[textureHandle_].resource.Get(), mipImages);

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	if (metadata.IsCubemap())
	{
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else
	{
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	}

	//SRVを作成するDescriptorHeapの場所を決める
	textures_[textureHandle_].textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap_.Get(), sDescriptorSizeSRV, textureHandle_);
	textures_[textureHandle_].textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap_.Get(), sDescriptorSizeSRV, textureHandle_);

	//SRVの作成
	device_->CreateShaderResourceView(textures_[textureHandle_].resource.Get(), &srvDesc, textures_[textureHandle_].textureSrvHandleCPU);

	textures_[textureHandle_].name = filePath;
	textures_[textureHandle_].textureHandle = textureHandle_;

	return textureHandle_;
}

DirectX::ScratchImage TextureManager::OpenImage(const std::string& filePath)
{
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr;
	if (filePathW.ends_with(L".dds"))
	{
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else
	{
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};

	if (DirectX::IsCompressed(image.GetMetadata().format))
	{
		mipImages = std::move(image);
	}
	else
	{
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
	}

	assert(SUCCEEDED(hr));

	//ミップマップ付きのデータを返す
	return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(const DirectX::TexMetadata& metadata)
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

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//Resourceの作成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr;
	hr = device_->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}


[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	//中間リソースを作成
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device_, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = DirectXCore::GetInstance()->CreateBufferResource(intermediateSize);

	//データ転送をコマンドに積む
	UpdateSubresources(commandList_, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	//Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList_->ResourceBarrier(1, &barrier);

	return intermediateResource;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, const uint32_t descriptorSize, uint32_t index)
{
	//CPUディスクリプタハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += static_cast<D3D12_CPU_DESCRIPTOR_HANDLE>((descriptorSize * index)).ptr;
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, const uint32_t descriptorSize, uint32_t index)
{
	//GPUディスクリプタハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += static_cast<D3D12_GPU_DESCRIPTOR_HANDLE>((descriptorSize * index)).ptr;
	return handleGPU;
}

const D3D12_RESOURCE_DESC TextureManager::GetResourceDesc(uint32_t textureHandle)
{
	//テクスチャリソースを取得
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc = textures_[textureHandle].resource->GetDesc();

	return resourceDesc;
}