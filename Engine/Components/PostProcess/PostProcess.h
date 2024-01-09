#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"
#include <array>
#include <dxcapi.h>
#include <vector>
#pragma comment(lib,"dxcompiler.lib")

class PostProcess 
{
public:
	static uint32_t descriptorSizeRTV;
	static uint32_t descriptorSizeSRV;
	static uint32_t descriptorSizeDSV;

	struct Texture 
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		uint32_t rtvIndex;
		uint32_t srvIndex;
	};

	struct VertexPosUV 
	{
		Vector4 pos;
		Vector2 texcoord;
	};

	struct BlurData 
	{
		int32_t textureWidth;
		int32_t textureHeight;
		float padding[2];
		float weight[8];
	};

	struct BloomData
	{
		bool enable;
		float padding[3];
	};

	struct VignetteData
	{
		bool enable;
		float intensity;
	};

	static PostProcess* GetInstance();

	void Initialize();

	void Update();

	void PreDraw();

	void PostDraw();

	void SetIsPostProcessActive(bool flag) { isPostProcessActive_ = flag; };

	void SetIsBloomActive(bool flag) { isBloomActive_ = flag; };

	void SetIsVignetteActive(bool flag) { isVignetteActive_ = flag; };

	void SetVignetteIntensity(float intensity) { vignetteIntensity_ = intensity; };

private:
	//Blurの種類
	enum BlurState 
	{
		kHorizontal,
		kVertical,
	};

	//DXCの初期化
	void InitializeDXC();

	//CompileShader
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	//PSOの作成
	void CreatePSO();
	void CreateBlurPSO();
	void CreatePostProcessPSO();

	//描画処理
	void Draw();
	void PreSecondPassDraw();
	void SecondPassDraw();
	void PostSecondPassDraw();

	//Blur
	void PreBlur(BlurState blurState);
	void Blur(BlurState blurState, uint32_t srvIndex, uint32_t highIntensitySrvIndex);
	void PostBlur(BlurState blurState);
	void PreShrinkBlur(BlurState blurState);
	void ShrinkBlur(BlurState blurState, uint32_t srvIndex, uint32_t highIntensitySrvIndex);
	void PostShrinkBlur(BlurState blurState);

	//Bloom
	void Bloom();

	//Vignette
	void Vignette();

	//マルチパス用テクスチャの作成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(uint32_t width, uint32_t height, DXGI_FORMAT format, const float* clearColor);

	//深度テクスチャの作成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(int32_t width, int32_t height);

	//RTVの作成
	uint32_t CreateRTV(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format);

	//SRVの作成
	uint32_t CreateSRV(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format);

	//DSVの作成
	void CreateDSV();

	//DescriptorHandleの取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, const uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, const uint32_t descriptorSize, uint32_t index);

private:
	DirectXCore* dxCore_ = nullptr;

	ID3D12GraphicsCommandList* commandList_;

	ID3D12Device* device_;

	//DXCompiler
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

	//RootSignature
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> blurRootSignature_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> postProcessRootSignature_;

	//PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, 2> blurPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> postProcessPipelineState_;

	//Vertex
	std::vector<VertexPosUV> vertices_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//DescriptorHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> multiPassRTVDescriptorHeap_ = nullptr;
	uint32_t rtvIndex_ = -1;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> multiPassSRVDescriptorHeap_ = nullptr;
	uint32_t srvIndex_ = -1;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> multiPassDSVDescriptorHeap_ = nullptr;

	//深度バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	//テクスチャ
	Texture firstPassResource_ = { nullptr };
	Texture secondPassResource_ = { nullptr };
	Texture linearDepthResource_ = { nullptr };
	Texture highIntensityResource_ = { nullptr };
	std::array<Texture, 2> blurResources_ = { nullptr };
	std::array<Texture, 2> highIntensityBlurResource_ = { nullptr };
	std::array<Texture, 2> shrinkBlurResources_ = { nullptr };
	std::array<Texture, 2> shrinkHighIntensityBlurResources_ = { nullptr };

	//Blur
	Microsoft::WRL::ComPtr<ID3D12Resource> blurConstantBuffer_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> shrinkBlurConstantBuffer_ = nullptr;

	//Bloom
	Microsoft::WRL::ComPtr<ID3D12Resource> bloomConstantBuffer_ = nullptr;

	//Vignette
	Microsoft::WRL::ComPtr<ID3D12Resource> vignetteConstantBuffer_ = nullptr;
	float vignetteIntensity_ = 1.5f;

	//Flag
	bool isPostProcessActive_ = false;
	bool isBloomActive_ = false;
	bool isVignetteActive_ = false;
};