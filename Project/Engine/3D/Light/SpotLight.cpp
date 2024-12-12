/**
 * @file SpotLight.cpp
 * @brief スポットライトの初期化、更新、ライティングの設定などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "SpotLight.h"

void SpotLight::Initialize()
{
	//DirectXCoreのインスタンスの取得
	dxCore_ = DirectXCore::GetInstance();

	//ライティングリソースの作成
	lightingResource_ = dxCore_->CreateBufferResource(sizeof(ConstBuffDataSpotLight));

	//ライティングデータを設定
	ConstBuffDataSpotLight* lightData = nullptr;
	lightingResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));
	lightData->enableLighting = enableLighting_;
	lightData->lightingType = lightingType_;
	lightData->modelType = modelType_;
	lightData->color = color_;
	lightData->direction = direction_;
	lightData->distance = distance_;
	lightData->position = position_;
	lightData->intensity = intensity_;
	lightData->decay = decay_;
	lightData->cosAngle = cosAngle_;
	lightData->cosFalloffStart = cosFalloffStart_;

	//定数バッファをアンマップ
	lightingResource_->Unmap(0, nullptr);
}

void SpotLight::Update()
{
	//定数バッファをマッピング
	ConstBuffDataSpotLight* lightData = nullptr;
	lightingResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));

	//ライティングデータを更新
	lightData->enableLighting = enableLighting_;
	lightData->lightingType = lightingType_;
	lightData->modelType = modelType_;
	lightData->color = color_;
	lightData->direction = direction_;
	lightData->distance = distance_;
	lightData->position = position_;
	lightData->intensity = intensity_;
	lightData->decay = decay_;
	lightData->cosAngle = cosAngle_;
	lightData->cosFalloffStart = cosFalloffStart_;

	//定数バッファをアンマップ
	lightingResource_->Unmap(0, nullptr);
}

void SpotLight::SetGraphicsCommand(UINT rootParameterIndex)
{
	//グラフィックコマンドリストに定数バッファのビューを設定
	dxCore_->GetCommandList()->SetGraphicsRootConstantBufferView(rootParameterIndex, lightingResource_->GetGPUVirtualAddress());
}

void SpotLight::ImGui()
{
	
}