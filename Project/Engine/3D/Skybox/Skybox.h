/**
 * @file Skybox.h
 * @brief Skyboxの生成、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/3D/Model/Material.h"
#include <cassert>
#include <dxcapi.h>
#include <array>

#pragma comment(lib,"dxcompiler.lib")

class Skybox
{
public:
	static const uint32_t kMaxVertices = 24;

	static const uint32_t kMaxIndices = 36;

	struct VertexPosUV
	{
		Vector4 position;
		Vector2 texcoord;
	};

	/*struct Material
	{
		Vector4 color;
	};*/

	enum class RootParameterIndex
	{
		Material,
		WorldTransform,
		ViewProjection,
		Texture,
	};

	static void StaticInitialize();

	static void Release();

	static void PreDraw();

	static void PostDraw();

	static Skybox* Create();

	void Draw(WorldTransform& worldTransform, const Camera& camera);

private:
	static void InitializeDXC();

	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	void Initialize();

	static void CreatePSO();

	void CreateVertexResource();

	void CreateIndexResource();

	void CreateMaterialResource();

	void UpdateMaterialResource();

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

	std::array<Vector4, kMaxVertices> vertices_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::array<uint32_t, kMaxIndices> indices_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	uint32_t textureHandle_ = 0;

	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
};

