/**
 * @file Light.cpp
 * @brief ライトの初期化、更新、ライティングの設定などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Light.h"

void Light::Initialize()
{
    //DirectXCoreのインスタンスの取得
    dxCore_ = DirectXCore::GetInstance();

    //ライティングリソースの作成
    lightingResource_ = dxCore_->CreateBufferResource(sizeof(ConstBuffDataLight));

    //定数バッファをマッピング
    ConstBuffDataLight* lightData = nullptr;
    lightingResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));

    //ライティングデータを設定
    lightData->enableLighting = enableLighting_;   
    lightData->lightingType = lightingType_;        
    lightData->modelType = modelType_;             
    lightData->color = color_;                      
    lightData->direction = direction_;             
    lightData->intensity = intensity_;              

    //定数バッファをアンマップ
    lightingResource_->Unmap(0, nullptr);
}

void Light::Update()
{
    //定数バッファをマッピング
    ConstBuffDataLight* lightData = nullptr;
    lightingResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));

    //ライティングデータを更新
    lightData->enableLighting = enableLighting_;   
    lightData->lightingType = lightingType_;       
    lightData->modelType = modelType_;             
    lightData->color = color_;                      
    lightData->direction = Normalize(direction_);  
    lightData->intensity = intensity_;              

    //定数バッファをアンマップ
    lightingResource_->Unmap(0, nullptr);
}

void Light::SetGraphicsCommand(UINT rootParameterIndex)
{
    //グラフィックコマンドリストに定数バッファのビューを設定
	dxCore_->GetCommandList()->SetGraphicsRootConstantBufferView(rootParameterIndex, lightingResource_->GetGPUVirtualAddress());
}

void Light::ImGui()
{
	
}
