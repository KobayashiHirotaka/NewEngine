#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/3D/Mesh/Mesh.h"
#include "Engine/3D/Model/Material.h"
#include "Engine/3D/Light/Light.h"
#include <cassert>
#include <dxcapi.h>
#include <fstream>
#include <list>
#include <string>
#include <sstream>
#include <span>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#pragma comment(lib,"dxcompiler.lib")

static const uint32_t kNumMaxInfluence = 4;

enum class RootParameterIndex
{
	Material,
	WorldTransform,
	ViewProjection,
	Texture,
	Light,
	Skinning
};

struct VertexInfluence
{
	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGPU
{
	Matrix4x4 skeletonSpaceMatrix;
	Matrix4x4 skeletonSpaceInverseTransposeMatrix;
};


struct SkinCluster
{
	std::vector<Matrix4x4> inverseBindPoseMatrices;
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence> mappedInfluence;
	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
	std::span<WellForGPU> mappedPalette;
	D3D12_GPU_DESCRIPTOR_HANDLE paletteSrvHandle;
};

class Model 
{
public:
	static void StaticInitialize();

	//void Initialize();

	void Update(WorldTransform& worldTransform);

	void Draw(WorldTransform& worldTransform, const Camera& camera);

	static void Release();

	static Model* CreateFromOBJ(const std::string& directoryPath, const std::string& filename);

	static void PreDraw();

	static void PostDraw();

	Material* GetMaterial() { return material_.get(); };

	Light* GetLight() { return light_.get(); };

	float GetAnimationTime() { return animationTime_; };

	void SetAnimationTime(float animationTime) { animationTime_ = animationTime; };

	Animation GetAnimation() { return animation_; };

	void ApplyAnimation();

private:
	static void InitializeDXC();

	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	static void CreatePSO();

	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	Node ReadNode(aiNode* node);

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

	Skeleton CreateSkelton(const Node& rootNode);

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	SkinCluster CreateSkinCluster(const Skeleton& skeleton,const ModelData& modelData);



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
	
	uint32_t textureHandle_;

	VertexData* vertexData_;

	ModelData modelData_;

	float animationTime_ = 0.0f;
	Animation animation_;
	bool isKeyframeAnimation_ = false;

	Skeleton skeleton_;

	SkinCluster skinCluster_;

	uint32_t skinningTextureHandle_;
};