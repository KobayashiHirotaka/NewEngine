#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/3D/Mesh/Mesh.h"
#include "Engine/3D/Model/Material.h"
#include "Engine/3D/Light/Light.h"
#include "Engine/3D/Light/PointLight.h"
#include <cassert>
#include <dxcapi.h>
#include <fstream>
#include <list>
#include <string>
#include <sstream>

#pragma comment(lib,"dxcompiler.lib")

enum class RootParameterIndex
{
	Material,
	WorldTransform,
	ViewProjection,
	Texture,
	Light,
	PointLight
};

class Model 
{
public:
	struct MaterialData
	{
		std::string textureFilePath;
	};

	struct ModelData 
	{
		std::vector<Mesh::VertexData> vertices;
		MaterialData material;
		std::string name;
	};

	struct ModelTransformationData
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	static void StaticInitialize();

	//void Initialize();

	void Draw(const WorldTransform& worldTransform, const Camera& camera);

	static void Release();

	static Model* CreateFromOBJ(const std::string& directoryPath, const std::string& filename);

	static void PreDraw();

	static void PostDraw();

	Material* GetMaterial() { return material_.get(); };

	Light* GetLight() { return light_.get(); };

	PointLight* GetPointLight() { return pointLight_.get(); };

private:
	static void InitializeDXC();

	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	static void CreatePSO();

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

	std::unique_ptr<Mesh> mesh_ = nullptr;
	
	std::unique_ptr<Material> material_ = nullptr;

	std::unique_ptr<Light>light_;

	std::unique_ptr<PointLight>pointLight_;
	
	uint32_t textureHandle_;
};