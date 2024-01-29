#pragma once
#include "Engine/Base/WindowsApp/WindowsApp.h"
#include "Engine/Utility/ConvertString/ConvertString.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <chrono>
#include <cassert>
#include <wrl.h>
#include <thread>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"winmm.lib")

class DirectXCore
{
public:
	static uint32_t descriptorSizeRTV;
	static uint32_t descriptorSizeDSV;

	static DirectXCore* GetInstance();

	static void DeleteInstance();

	void Initialize();

	void PreDraw();

	void PostDraw();

	void SetBackBuffer();

	void ClearDepthBuffer();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	ID3D12Device* GetDevice() const { return device_.Get(); };

	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

private:
	void CreateDXGIDevice();

	void CreateCommand();

	void CreateSwapChain();

	void CreateRTV();

	void CreateDSV();

	void CreateFence();
	
	void CreateViewport();

	void CreateScissorRect();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(int32_t width, int32_t height);

private:
	DirectXCore() = default;
	~DirectXCore() = default;
	DirectXCore(const DirectXCore&) = delete;
	DirectXCore& operator=(const DirectXCore&) = delete;

	void InitializeFixFPS();
	void UpdateFixFPS();

private:
	static DirectXCore* instance_;

	WindowsApp* win_ = nullptr;

	//DXGIファクトリーの生成
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	//使用するアダプタ用の変数
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;

	//D3D12Deviceの生成
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	//コマンドキュー生成
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	//コマンドアロケータの生成
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;

	//コマンドリストを生成する
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	//スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	//ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;

	//リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	uint64_t fenceValue_{};

	//レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};

	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_{};
};