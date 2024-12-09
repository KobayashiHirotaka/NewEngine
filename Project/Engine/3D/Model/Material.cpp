/**
 * @file Material.cpp
 * @brief マテリアルの初期化、更新などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Material.h"

void Material::Initialize()
{
	dxCore_ = DirectXCore::GetInstance();

	materialResource_ = dxCore_->CreateBufferResource(sizeof(ConstBufferDataMaterial));

	ConstBufferDataMaterial* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = color_;
	materialData->uvTransform = MakeIdentity4x4();
	materialData->shininess = shininess_;
	materialData->environmentCofficient = environmentCofficient_;
	materialResource_->Unmap(0, nullptr);
}

void Material::Update()
{
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(Vector3{ scale_.x,scale_.y,1.0f });
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(rotation_));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(Vector3{ translation_.x,translation_.y,0.0f }));

	ConstBufferDataMaterial* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = color_;
	materialData->uvTransform = uvTransformMatrix;
	materialData->shininess = shininess_;
	materialData->environmentCofficient = environmentCofficient_;
	materialResource_->Unmap(0, nullptr);
}

void Material::SetGraphicsCommand(UINT rootParameterIndex)
{
	dxCore_->GetCommandList()->SetGraphicsRootConstantBufferView(rootParameterIndex, materialResource_->GetGPUVirtualAddress());
}

void Material::ImGui()
{
	ImGui::Begin("Material");
	ImGui::SliderFloat("EnvironmentCofficient", &environmentCofficient_, 0.0f, 1.0f);
	ImGui::End();
}