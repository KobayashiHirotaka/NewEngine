#include "Light.h"

void Light::Initialize() 
{
	lightingResource_ = DirectXCore::GetInstance()->CreateBufferResource(sizeof(ConstBufferDataDirectionalLight));

	ConstBufferDataDirectionalLight* directionalLightData = nullptr;
	lightingResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->lightingMethod = lightingMethod_;
	directionalLightData->enableLighting = enableLighting_;
	directionalLightData->color = color_;
	directionalLightData->direction = direction_;
	directionalLightData->intensity = intensity_;
	lightingResource_->Unmap(0, nullptr);
}

void Light::Update() 
{
	//lightingResourceに書き込む
	ConstBufferDataDirectionalLight* directionalLightData = nullptr;
	lightingResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->lightingMethod = lightingMethod_;
	directionalLightData->enableLighting = enableLighting_;
	directionalLightData->color = color_;
	directionalLightData->direction = direction_;
	directionalLightData->intensity = intensity_;
	lightingResource_->Unmap(0, nullptr);
}

void Light::SetGraphicsCommand(UINT rootParameterIndex)
{
	//lightingResourceの場所を設定
	DirectXCore::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(rootParameterIndex, lightingResource_->GetGPUVirtualAddress());
}