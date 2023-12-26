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

	static void PreDraw(BlendMode blendMode);

	static void PostDraw();

	static void Release();

	void Draw();

	static Sprite* Create(uint32_t textureHandle, Vector2 position);

	const Vector2& GetPosition() const { return position_; };

	void SetPosition(const Vector2& position) { position_ = position; };

	const float& GetRotation() const { return rotation_; };

	void SetRotation(const float& rotation) { rotation_ = rotation; };

	const Vector2& GetSize() const { return size_; };

	void SetSize(const Vector2& size) { size_ = size; };

	const Vector4& GetColor() const { return color_; };

	void SetColor(const Vector4& color) { color_ = color; };

private:
	static void InitializeDXC();

	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	static void CreatePipelineStateObject();

	void Initialize(uint32_t textureHandle, Vector2 position);

	void CreateVertexBuffer();

	void CreateMaterialResource();

	void UpdateMaterial();

	void CreateWVPResource();

	void UpdateMatrix();

private:
	static ID3D12Device* sDevice_;
	
	static ID3D12GraphicsCommandList* sCommandList_;
	
	static Microsoft::WRL::ComPtr<IDxcUtils> sDxcUtils_;
	static Microsoft::WRL::ComPtr<IDxcCompiler3> sDxcCompiler_;
	static Microsoft::WRL::ComPtr<IDxcIncludeHandler> sIncludeHandler_;

	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;

	static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfBlendMode> sGraphicsPipelineState_;
	
	static Matrix4x4 sMatProjection_;
	
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
};
