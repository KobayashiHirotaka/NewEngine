#include "Light.h"

void Light::Initialize() 
{
	dxCore_ = DirectXCore::GetInstance();

	lightingResource_ = dxCore_->CreateBufferResource(sizeof(ConstBuffDataLight));

	ConstBuffDataLight* lightData = nullptr;
	lightingResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));
	lightData->enableLighting = enableLighting_;
	lightData->lightingType = lightingType_;
	lightData->color = color_;
	lightData->direction = direction_;
	lightData->intensity = intensity_;
	lightingResource_->Unmap(0, nullptr);
}

void Light::Update() 
{
	ConstBuffDataLight* lightData = nullptr;
	lightingResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));
	lightData->enableLighting = enableLighting_;
	lightData->lightingType = lightingType_;
	lightData->color = color_;
	lightData->direction = Normalize(direction_);
	lightData->intensity = intensity_;
	lightingResource_->Unmap(0, nullptr);
}

void Light::SetGraphicsCommand(UINT rootParameterIndex)
{
	dxCore_->GetCommandList()->SetGraphicsRootConstantBufferView(rootParameterIndex, lightingResource_->GetGPUVirtualAddress());
}

void Light::ImGui(const char* Title)
{
	ImGui::Begin(Title);
	ImGui::DragFloat4("color", &color_.x, 1.0f, 0.0f, 255.0f);
	ImGui::DragFloat3("direction", &direction_.x, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat("intensity", &intensity_, 0.1f, 0.0f, 100.0f);
	ImGui::End();
}
