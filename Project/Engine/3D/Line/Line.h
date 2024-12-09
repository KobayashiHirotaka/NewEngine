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
	static void StaticInitialize();

	void Update(Vector4 start, Vector4 end);

	void Draw(WorldTransform& worldTransform, const Camera& camera);

	static Line* Create(Vector4 start, Vector4 end);

	static void Release();

	static void PreDraw();

	static void PostDraw();

private:
	static void InitializeDXC();

	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	static void CreatePSO();

	void CreateVertexBuffer();

	void UpdateVertexBuffer();

private:
	static DirectXCore* dxCore_;

	static TextureManager* textureManager_;

	static ID3D12Device* device_;

	static ID3D12GraphicsCommandList* commandList_;

	static Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	static Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	static Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::vector<Vector4> vertices_{};

	static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	static Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
};

