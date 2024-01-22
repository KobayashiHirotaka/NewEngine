#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/Utility/Math/MyMath.h"
#include <array>
#include <dxcapi.h>

#pragma comment(lib,"dxcompiler.lib")

class Sprite 
{
public:
	enum BlendMode 
	{
		kBlendModeNone,
	
		kBlendModeNormal,
	
		kBlendModeAdd,
	
		kBlendModeSubtract,
		
		kBlendModeMultiply,
	
		kBlendModeScreen,
		
		kCountOfBlendMode,
	};

	struct VertexData 
	{
		Vector4 position{};
		Vector2 texcoord{};
	};

	struct MaterialData 
	{
		Vector4 color{};
		Matrix4x4 uvTransform{};
	};

	static void StaticInitialize();

	void Initialize(uint32_t textureHandle, Vector2 position);

	void Update();

	void Draw();

	static void Release();

	static Sprite* Create(uint32_t textureHandle, Vector2 position);

	static void PreDraw(BlendMode blendMode);

	static void PostDraw();

	void ImGui(const char* Title);

	//Position
	const Vector2& GetPosition() const { return position_; };
	void SetPosition(const Vector2& position) { position_ = position; };

	//Rotation
	const float& GetRotation() const { return rotation_; };
	void SetRotation(const float& rotation) { rotation_ = rotation; };

	//Size
	const Vector2& GetSize() const { return size_; };
	void SetSize(const Vector2& size) { size_ = size; };

	//Color
	const Vector4& GetColor() const { return color_; };
	void SetColor(const Vector4& color) { color_ = color; };

	//拡張機能
	//AnchorPoint
	const Vector2& GetAnchorPoint() const { return anchorPoint_; };
	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; };

	//Flip
	bool GetIsFlipX() { return isFlipX_; };
	void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; };

	bool GetIsFlipY() { return isFlipY_; };
	void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; };

	//TextureLeftTop
	const Vector2& GetTextureLeftTop() const { return textureLeftTop_; };
	void SetTextureLeftTop(const Vector2& textureLeftTop) { textureLeftTop_ = textureLeftTop; };

	//TextureSize
	const Vector2& GetTextureSize() const { return textureSize_; };
	void SetTextureSize(const Vector2& textureSize) { textureSize_ = textureSize; };

private:
	static void InitializeDXC();

	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	static void CreatePSO();

	void CreateVertexBuffer();

	void CreateMaterialResource();

	void CreateWVPResource();

	void UpdateMaterial();

	void UpdateMatrix();

	void AdjustTextureSize();

private:
	static DirectXCore* dxCore_;

	static TextureManager* textureManager_;

	static ID3D12Device* device_;
	
	static ID3D12GraphicsCommandList* commandList_;
	
	static Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	static Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	static Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

	static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfBlendMode> graphicsPipelineState_;
	
	static Matrix4x4 matProjection_;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	
	Vector2 position_ = { 0.0f,0.0f };
	
	float rotation_ = 0.0f;
	
	Vector2 size_ = { 1.0f,1.0f };
	
	Vector2 texBase_ = { 0.0f,0.0f };
	
	Vector2 texSize_ = { 1.0f,1.0f };
	
	uint32_t textureHandle_{};
	
	D3D12_RESOURCE_DESC resourceDesc_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_ = nullptr;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector2 uvTranslation_ = { 0.0f,0.0f };
	
	float uvRotation_ = 0.0f;

	Vector2 uvScale_ = { 1.0f,1.0f };

	Vector2 anchorPoint_{ 0.0f,0.0f };

	bool isFlipX_ = false;
	bool isFlipY_ = false;

	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	Vector2 textureSize_ = { 100.0f,100.0f };
};
