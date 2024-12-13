/**
 * @file Line.h
 * @brief 線の生成、更新、描画などを行う
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
#include <list>
#include <string>

#pragma comment(lib,"dxcompiler.lib")

class Line
{
public:
	/// <summary>初期化</summary>
	static void StaticInitialize();

	/// <summary>更新</summary>
	void Update(Vector4 start, Vector4 end);

	/// <summary>描画</summary>
	void Draw(WorldTransform& worldTransform, const Camera& camera);

	/// <summary>線の生成</summary>
	static Line* Create(Vector4 start, Vector4 end);

	/// <summary>解放</summary>
	static void Release();

	/// <summary>描画前の処理</summary>
	static void PreDraw();

	/// <summary>描画後の処理</summary>
	static void PostDraw();

private:
	/// <summary>DXCの初期化</summary>
	static void InitializeDXC();

	/// <summary>コンパイルシェーダー</summary>
	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	/// <summary>PSOの生成</summary>
	static void CreatePSO();

	/// <summary>頂点バッファの生成</summary>
	void CreateVertexBuffer();

	/// <summary>頂点バッファの更新</summary>
	void UpdateVertexBuffer();

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

	//頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	std::vector<Vector4> vertices_{};

	//パイプライン関連
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sGraphicsPipelineState_;
};

