#pragma once
#include "Engine/Base/WindowsApp/WindowsApp.h"
#include "Engine/Utility/ConvertString/ConvertString.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/externals/DirectXTex/d3dx12.h"
#include "dxgidebug.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <wrl.h>
#include <thread>
#include <chrono>
#include <cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"dxguid.lib")

class DirectXCore
{
public:
	static DirectXCore* GetInstance();

	void Initialize(WindowsApp* win);

	void PreDraw();

	void PostDraw();

	void Release();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	ID3D12Device* GetDevice() { return device_.Get(); }

	ID3D12GraphicsCommandList* GetcommandList() { return commandList_.Get(); }

	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc_; }

	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc_; }

	ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return srvDescriptorHeap_.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device,
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	void CreateDXGIDevice();

	void CreateCommand();

	void CreateSwapChain();

	void CreateFinalRenderTargets();

	void CreateFence();

	void CreateRootSignature();

	void CreateInputLayOut();

	void CreateBlendState();

	void CreateRasterizerState();

	void CreatePSO();

	void CreateViewport();

	void CreateScissorRect();

	void CreateDxcCompiler();
	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(int32_t width, int32_t height);

private:
	void InitializeFixFPS();

	void UpdateFixFPS();

private:
	enum BlendMode
	{
		kBlendModeNone,
		kBlendModeNormal,
		kBlendModeAdd,
		kBlendModeSubtract,
		kBlendModeMultiply,
		kBlendModeScreen,
		kCountOfBlendMode
	};

	WindowsApp* win_;

	HRESULT hr_;

	//DXGIファクトリーの生成
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;

	//使用するアダプタ用の変数
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;

	//D3D12Deviceの生成
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	//コマンドキュー生成
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;

	//コマンドアロケータの生成
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;

	//コマンドリストを生成する
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

	//スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_{};
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;

	//ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

	//深度
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};

	//RTVを２つ作るのでディスクリプタを２つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];

	//Fence
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	UINT64 fenceValue_;
	HANDLE fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);

	D3D12_RESOURCE_BARRIER barrier_{};

	//DXCの初期化
	IDxcUtils* dxcUtils_ = nullptr;
	IDxcCompiler3* dxcCompiler_ = nullptr;
	IDxcIncludeHandler* includeHandler_ = nullptr;

	IDxcBlobUtf8* shaderError_ = nullptr;
	IDxcBlob* shaderBlob_ = nullptr;

	//inputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3];

	//blend
	D3D12_BLEND_DESC blendDesc_{};

	//rasterizer
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	//PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	ID3DBlob* signatureBlob_;
	ID3DBlob* errorBlob_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	IDxcBlob* vertexShaderBlob_;

	IDxcBlob* pixelShaderBlob_;

	//viewport
	D3D12_VIEWPORT viewport_{};

	//scissor
	D3D12_RECT scissorRect_{};

	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_{};
	
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_ = nullptr;
#endif
};


