/**
 * @file PointLight.cpp
 * @brief ポイントライトの初期化、更新、ライティングの設定などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "PointLight.h"

void PointLight::Initialize()
{
	//DirectXCoreのインスタンスの取得
	dxCore_ = DirectXCore::GetInstance();

	//ライティングリソースの作成
	lightingResource_ = dxCore_->CreateBufferResource(sizeof(ConstBuffDataPointLight));

	//定数バッファをマッピング
	ConstBuffDataPointLight* lightData = nullptr;
	lightingResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));

	//ライティングデータを設定
	lightData->enableLighting = enableLighting_;
	lightData->lightingType = lightingType_;
	lightData->modelType = modelType_;
	lightData->color = color_;
	lightData->direction = direction_;
	lightData->intensity = intensity_;
	lightData->radius = radius_;
	lightData->decay = decay_;

	//定数バッファをアンマップ
	lightingResource_->Unmap(0, nullptr);
}

void PointLight::Update()
{
	//定数バッファをマッピング
	ConstBuffDataPointLight* lightData = nullptr;
	lightingResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));

	//ライティングデータを更新
	lightData->enableLighting = enableLighting_;
	lightData->lightingType = lightingType_;
	lightData->modelType = modelType_;
	lightData->color = color_;
	lightData->direction = direction_;
	lightData->intensity = intensity_;
	lightData->radius = radius_;
	lightData->decay = decay_;

	//定数バッファをアンマップ
	lightingResource_->Unmap(0, nullptr);
}

void PointLight::SetGraphicsCommand(UINT rootParameterIndex)
{
	//コマンドリストに定数バッファのビューを設定
	dxCore_->GetCommandList()->SetGraphicsRootConstantBufferView(rootParameterIndex, lightingResource_->GetGPUVirtualAddress());
}

void PointLight::ImGui()
{
	
}
