/**
 * @file Model.h
 * @brief モデルの読み込み、生成、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

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
#include "Engine/3D/Light/SpotLight.h"
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

//最大影響数
static const uint32_t kNumMaxInfluence = 4;

//ルートパラメータインデックス
enum class RootParameterIndex
{
	//マテリアル
	Material,

	//WorldTransform
	WorldTransform,

	//ViewProjection
	ViewProjection,

	//テクスチャ
	Texture,

	//ライト
	Light,

	//ポイントライト
	PointLight,

	//スポットライト
	SpotLight,

	//スキニング
	Skinning,
};

//頂点の影響
struct VertexInfluence
{
	//各関節の影響度
	std::array<float, kNumMaxInfluence> weights;

	//関節のインデックス
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

//GPU用のデータ
struct WellForGPU
{
	//骨格空間行列
	Matrix4x4 skeletonSpaceMatrix;

	//骨格空間の逆転置行列
	Matrix4x4 skeletonSpaceInverseTransposeMatrix;
};

//スキンクラスター
struct SkinCluster
{
	//逆バインドポーズ行列
	std::vector<Matrix4x4> inverseBindPoseMatrices;

	//影響リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;

	//影響バッファビュー
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;

	//マッピングされた影響データ
	std::span<VertexInfluence> mappedInfluence;

	//パレットリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;

	//マッピングされたパレットデータ
	std::span<WellForGPU> mappedPalette;

	//パレットSRVハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE paletteSrvHandle;
};

class Model
{
public:
	/// <summary>静的初期化</summary>
	static void StaticInitialize();

	/// <summary>更新</summary>
	void Update();

	/// <summary>描画</summary>
	void Draw(WorldTransform& worldTransform, const Camera& camera, const uint32_t animationData);

	/// <summary>骨の描画</summary>
	void DrawBone(WorldTransform& worldTransform, const Camera& camera);

	/// <summary>解放</summary>
	static void Release();

	/// <summary>OBJの生成</summary>
	static Model* CreateFromOBJ(const std::string& directoryPath, const std::string& filename);

	/// <summary>描画前の処理</summary>
	static void PreDraw();

	/// <summary>描画後の処理</summary>
	static void PostDraw();

	/// <summary>骨の描画前の処理</summary>
	static void PreDrawBone();

	/// <summary>骨の描画後の処理</summary>
	static void PostDrawBone();

	/// <summary>アニメーションの適応</summary>
	void ApplyAnimation(const uint32_t animationData);

	/// <summary>JointWorldTransformの取得</summary>
	WorldTransform& GetJointWorldTransform(const std::string& jointName);

	//Getter
	//Material
	Material* GetMaterial() { return material_.get(); };

	//Light
	Light* GetLight() { return light_.get(); };

	//PointLight
	PointLight* GetPointLight() { return pointLight_.get(); };

	//SpotLight
	SpotLight* GetSpotLight() { return spotLight_.get(); };

	//AnimationTime
	float GetAnimationTime() { return animationTime_; };

	//Animation
	std::vector<Animation> GetAnimation() const { return animation_; };

	//Setter
	//AnimationTime
	void SetAnimationTime(float animationTime) { animationTime_ = animationTime; };

private:
	/// <summary>DXCの初期化</summary>
	static void InitializeDXC();

	/// <summary>シェーダーコンパイル</summary>
	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	/// <summary>PSOの作成</summary>
	static void CreatePSO();

	/// <summary>モデルファイルの読み込み</summary>
	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>アニメーションファイルの読み込み</summary>
	std::vector<Animation> LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>マテリアルテンプレートファイルの読み込み</summary>
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>ノードを読み込む</summary>
	Node ReadNode(aiNode* node);

	/// <summary>キーフレームから値を計算</summary>
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

	/// <summary>スケルトンの作成</summary>
	Skeleton CreateSkelton(const Node& rootNode);

	/// <summary>ジョイントの作成</summary>
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	/// <summary>スキンクラスターの作成</summary>
	SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData);

	/// <summary>骨用の頂点バッファの作成</summary>
	void CreateBoneVertexBuffer();

	/// <summary>骨用の頂点の作成</summary>
	void CreateBoneVertices(const Skeleton& skeleton, int32_t index, std::vector<Vector4>& vertices);

	/// <summary>骨用の頂点の更新</summary>
	void UpdateBoneVertices(const Skeleton& skeleton, int32_t index, std::vector<Vector4>& vertices);

	/// <summary>骨用のPSOの作成</summary>
	static void CreateBonePSO();

private:
	//DirectXCoreのポインタ
	static DirectXCore* sDxCore_;

	//TextureManagerのポインタ
	static TextureManager* sTextureManager_;

	//DirectX初期化関連
	static ID3D12Device* sDevice_;
	static ID3D12GraphicsCommandList* sCommandList_;
	static Microsoft::WRL::ComPtr<IDxcUtils> sDxcUtils_;
	static Microsoft::WRL::ComPtr<IDxcCompiler3> sDxcCompiler_;
	static Microsoft::WRL::ComPtr<IDxcIncludeHandler> sIncludeHandler_;

	//パイプライン関連
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sGraphicsPipelineState_;

	//モデルデータ
	static std::list<ModelData> sModelDatas_;

	//メッシュ
	std::unique_ptr<Mesh> mesh_ = nullptr;

	//マテリアル
	std::unique_ptr<Material> material_ = nullptr;

	//ライト関連
	std::unique_ptr<Light>light_;
	std::unique_ptr<PointLight>pointLight_;
	std::unique_ptr<SpotLight>spotLight_;

	//テクスチャハンドル
	uint32_t textureHandle_;

	//頂点データ
	VertexData* vertexData_;

	//モデルデータ
	ModelData modelData_;

	//アニメーション関連
	float animationTime_ = 0.0f;
	std::vector<Animation> animation_;
	bool isKeyframeAnimation_ = false;

	//スケルトン
	Skeleton skeleton_;

	//スキンクラスター
	SkinCluster skinCluster_;

	//スキニング用テクスチャハンドル
	uint32_t skinningTextureHandle_;

	//骨用の頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> boneVertexBuffer_ = nullptr;

	//骨用の頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW boneVertexBufferView_{};

	//骨用の頂点
	std::vector<Vector4> boneVertices_{};

	//骨用のパイプライン関連
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sBoneRootSignature_;
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sBoneGraphicsPipelineState_;

	//デフォルトのWorldTransform
	WorldTransform defaultTransform_;

	//ジョイントのWorldTransform
	std::vector<WorldTransform> jointWorldTransform_{};

	//環境テクスチャハンドル
	static uint32_t sEnvironmentTextureHandle_;
};