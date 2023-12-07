#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Light/Light.h"
#include "Engine/Utility/Structs/VertexData.h"
#include "Engine/Utility/Structs/Material.h"
#include "Engine/Utility/Structs/ModelData.h"
#include <d3d12.h>
#include <fstream>
#include <sstream>
#include <string>
#include <wrl.h>

class Model
{
public:
	void Initialize(const std::string& directoryPath, const std::string& filename);
	void Draw(const WorldTransform& transform, const Camera& camera);
	void ImGui(const char* Title);

	static Model* CreateModelFromObj(const std::string& directoryPath, const std::string& filename);

	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	void SetColor(Vector4 color)
	{
		materialDataObj_->color = color;
	}

	void SetLighting(int32_t lightFlag)
	{
		materialDataObj_->enableLighting = lightFlag;
	}

private:
	DirectXCore* dxCore_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	Light* light_ = nullptr;

	int32_t lightFlag = Lighting::checkLighting;
	
	ModelData modelData_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceObj_;

	VertexData* vertexDataObj_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewObj_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceObj_ = nullptr;

	Material* materialDataObj_ = nullptr;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
};

