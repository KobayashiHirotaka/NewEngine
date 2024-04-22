#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "ParticleSystem.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Math/MyMath.h"
#include <dxcapi.h>
#include <fstream>
#include <list>
#include <string>
#include <sstream>
#pragma comment(lib,"dxcompiler.lib")

class ParticleModel 
{
public:
	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct MaterialData
	{
		std::string textureFilePath;
	};

	struct ModelData 
	{
		std::vector<VertexData> vertices;
		MaterialData material;
		std::string name;
	};

	struct ConstBuffDataMaterial
	{
		Vector4 color{};
		Matrix4x4 uvTransform{};
	};

	struct ParticleForGPU 
	{
		Matrix4x4 world;
		Vector4 color;
	};

	static void StaticInitialize();

	static void Release();

	static void PreDraw();

	static void PostDraw();

	static ParticleModel* CreateFromOBJ(const std::string& directoryPath, const std::string& filename);

	void Draw(const ParticleSystem* particleSystem, const Camera& camera);

private:
	static void InitializeDXC();

	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	void Initialize(const ModelData& modelData);

	static void CreatePSO();

	void CreateVertexResource();

	void CreateMaterialResource();

	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

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

	static std::list<ModelData> modelDatas_;
	
	std::vector<VertexData> vertices_{};
	
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	
	uint32_t textureHandle_ = 0;
};

