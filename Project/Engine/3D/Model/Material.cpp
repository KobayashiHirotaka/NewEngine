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
	materialResource_->Unmap(0, nullptr);
}

void Material::SetGraphicsCommand(UINT rootParameterIndex)
{
	dxCore_->GetCommandList()->SetGraphicsRootConstantBufferView(rootParameterIndex, materialResource_->GetGPUVirtualAddress());
}