#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/3D/Model/Material.h"
#include <cassert>
#include <dxcapi.h>
#include <fstream>
#include <list>
#include <string>
#include <sstream>
#include <span>

#pragma comment(lib,"dxcompiler.lib")

class Skybox
{
public:
	struct VertexData
	{
		Vector4 position;
	};

	struct MaterialData
	{
		std::string textureFilePath;
	};

	enum class RootParameterIndex
	{
		Material,
		WorldTransform,
		ViewProjection,
		Texture,
		Light,
		PointLight,
		SpotLight,
		Skinning,
	};

	static void StaticInitialize();

	static void Release();

	static void PreDraw();

	static void PostDraw();

	static Skybox* Create(const std::string& directoryPath, const std::string& filename);

	void Draw(WorldTransform& worldTransform, const Camera& camera);

private:
	static void InitializeDXC();

	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	void Initialize(const ModelData& modelData);

	static void CreatePSO();

	void CreateVertexResource();

	void CreateMaterialResource();

private:
	static DirectXCore* dxCore_;

	static TextureManager* textureManager_;

	static ID3D12Device* device_;

	static ID3D12GraphicsCommandList* commandList_;

	static Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	static Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	static Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

	static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	static Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	std::vector<VertexData> vertices_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	uint32_t textureHandle_ = 0;
};

