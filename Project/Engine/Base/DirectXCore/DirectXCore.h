/**
 * @file DirectXCore.h
 * @brief DirectX12の初期化、リソース管理、描画処理などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

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
	//RTV用のディスクリプタサイズ
	static uint32_t sDescriptorSizeRTV;

	//DSV用のディスクリプタサイズ
	static uint32_t sDescriptorSizeDSV;

	/// <summary>インスタンスの取得</summary>
	static DirectXCore* GetInstance();

	/// <summary>インスタンスの削除</summary>
	static void DeleteInstance();

	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>描画前の処理</summary>
	void PreDraw();

	/// <summary>描画後の処理</summary>
	void PostDraw();

	/// <summary>バックバッファの設定</summary>
	void SetBackBuffer();

	/// <summary>深度バッファのクリア</summary>
	void ClearDepthBuffer();

	/// <summary>バッファリソースを作成</summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	//Getter
	//DirectXデバイス
	ID3D12Device* GetDevice() const { return device_.Get(); };

	//コマンドリスト
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); };

	//ディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

private:
	/// <summary>DXGIデバイスを作成</summary>
	void CreateDXGIDevice();

	/// <summary>コマンドを作成</summary>
	void CreateCommand();

	/// <summary>スワップチェーンを作成</summary>
	void CreateSwapChain();

	/// <summary>RTVを作成</summary>
	void CreateRTV();

	/// <summary>DSVを作成</summary>
	void CreateDSV();

	/// <summary>フェンスを作成</summary>
	void CreateFence();

	/// <summary>ビューポートを作成</summary>
	void CreateViewport();

	/// <summary>シザー矩形を作成</summary>
	void CreateScissorRect();

	/// <summary>深度ステンシルテクスチャリソースを作成</summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(int32_t width, int32_t height);

private:
	//シングルトン
	DirectXCore() = default;
	~DirectXCore() = default;
	DirectXCore(const DirectXCore&) = delete;
	DirectXCore& operator=(const DirectXCore&) = delete;

	/// <summary>FPS固定のための初期化</summary>
	void InitializeFixFPS();

	/// <summary>FPS固定のための更新</summary>
	void UpdateFixFPS();

private:
	//DirectXCoreのインスタンス
	static DirectXCore* sInstance_;

	//WindowsAppのポインタ
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