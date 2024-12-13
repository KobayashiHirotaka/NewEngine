/**
 * @file Material.cpp
 * @brief マテリアルの初期化、更新などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Material.h"

void Material::Initialize()
{
	//DirectXCoreのインスタンスの取得
	dxCore_ = DirectXCore::GetInstance();

	//マテリアルリソースの作成
	materialResource_ = dxCore_->CreateBufferResource(sizeof(ConstBufferDataMaterial));

	//定数バッファをマッピング
	ConstBufferDataMaterial* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	//マテリアルデータを設定
	materialData->color = color_;
	materialData->uvTransform = MakeIdentity4x4();
	materialData->shininess = shininess_;
	materialData->environmentCofficient = environmentCofficient_;

	//定数バッファをアンマップ
	materialResource_->Unmap(0, nullptr);
}

void Material::Update()
{
	//UV座標を変換
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(Vector3{ scale_.x,scale_.y, scale_.z });
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(rotation_));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(Vector3{ translation_.x,translation_.y,0.0f }));

	//定数バッファをマッピング
	ConstBufferDataMaterial* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	//マテリアルデータを設定
	materialData->color = color_;
	materialData->uvTransform = uvTransformMatrix;
	materialData->shininess = shininess_;
	materialData->environmentCofficient = environmentCofficient_;

	//定数バッファをアンマップ
	materialResource_->Unmap(0, nullptr);
}

void Material::SetGraphicsCommand(UINT rootParameterIndex)
{
	//コマンドリストに定数バッファのビューを設定
	dxCore_->GetCommandList()->SetGraphicsRootConstantBufferView(rootParameterIndex, materialResource_->GetGPUVirtualAddress());
}

void Material::ImGui()
{

}