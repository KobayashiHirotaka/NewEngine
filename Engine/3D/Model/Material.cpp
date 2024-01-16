#include "Material.h"

void Material::Initialize() 
{
	//マテリアルリソースの作成
	materialResource_ = DirectXCore::GetInstance()->CreateBufferResource(sizeof(ConstBufferDataMaterial));

	//マテリアルリソースに書き込む
	ConstBufferDataMaterial* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = color_;
	materialData->uvTransform = MakeIdentity4x4();
	materialResource_->Unmap(0, nullptr);
}

void Material::Update() 
{
	//行列の作成
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(Vector3{ scale_.x,scale_.y,1.0f });
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(rotation_));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(Vector3{ translation_.x,translation_.y,0.0f }));

	//マテリアルリソースに書き込む
	ConstBufferDataMaterial* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = color_;
	materialData->uvTransform = uvTransformMatrix;
	materialResource_->Unmap(0, nullptr);
}

void Material::SetGraphicsCommand(UINT rootParameterIndex)
{
	//マテリアルリソースを設定
	DirectXCore::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(rootParameterIndex, materialResource_->GetGPUVirtualAddress());
}