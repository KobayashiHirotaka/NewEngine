/**
 * @file ParticleModel.h
 * @brief パーティクルモデルの読み込み、生成、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "ParticleSystem.h"
#include "Engine/3D/Model/Model.h"
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
	//頂点データ
	struct VertexData
	{
		//頂点位置
		Vector4 position{};

		//テクスチャ座標
		Vector2 texcoord{};

		//法線ベクトル
		Vector3 normal{};
	};

	//マテリアルデータ
	struct MaterialData
	{
		//テクスチャのファイルパス
		std::string textureFilePath;
	};

	//モデルデータ
	struct ModelData
	{
		//頂点
		std::vector<VertexData> vertices;

		//マテリアル
		MaterialData material;

		//名前
		std::string name;
	};

	//マテリアルの定数バッファデータ
	struct ConstBuffDataMaterial
	{
		//色
		Vector4 color{};

		//UV変換行列
		Matrix4x4 uvTransform{};
	};

	//GPUに送信するパーティクルのデータ
	struct ParticleForGPU
	{
		//ワールド行列
		Matrix4x4 world;

		//色
		Vector4 color;
	};

	/// <summary>静的初期化</summary>
	static void StaticInitialize();

	/// <summary>描画</summary>
	void Draw(const ParticleSystem* particleSystem, const Camera& camera);

	/// <summary>解放</summary>
	static void Release();

	/// <summary>描画前の処理</summary>
	static void PreDraw();

	/// <summary>描画後の処理</summary>
	static void PostDraw();

	/// <summary>OBJの作成</summary>
	static ParticleModel* CreateFromOBJ(const std::string& directoryPath, const std::string& filename);

private:
	/// <summary>DXCの初期化</summary>
	static void InitializeDXC();

	/// <summary>シェーダーコンパイル</summary>
	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	/// <summary>PSOの作成</summary>
	static void CreatePSO();

	/// <summary>初期化</summary>
	void Initialize(const ModelData& modelData);

	/// <summary>頂点リソースの作成</summary>
	void CreateVertexResource();

	/// <summary>マテリアルリソースの作成</summary>
	void CreateMaterialResource();

	/// <summary>OBJファイルの読み込み</summary>
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>マテリアルテンプレートファイルの読み込み</summary>
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

private:
	//DirectXCoreのポインタ
	static DirectXCore* sDxCore_;

	//TextureManagerのポインタ
	static Engine::TextureManager* sTextureManager_;

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

	//頂点関連
	std::vector<VertexData> vertices_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
};

