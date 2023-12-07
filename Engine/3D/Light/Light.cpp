#include "Light.h"

Light* Light::GetInstance()
{
	static Light instance;
	return &instance;
}

void Light::Initialize()
{
	dxCore_ = DirectXCore::GetInstance();

	directionalLightResource_ = dxCore_->CreateBufferResource(sizeof(DirectionalLight));
	directionalLightResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_->intensity = 1.0f;
}

void Light::ImGui(const char* Title)
{
	ImGui::Begin(Title);
	ImGui::SliderFloat3("LightColor", &directionalLightData_->color.x, 0, 1, "%.3f");
	ImGui::SliderFloat3("Lightpotision", &directionalLightData_->direction.x, -10, 10, "%.3f");
	ImGui::SliderFloat3("Lightintensity", &directionalLightData_->intensity, 0, 3, "%.3f");
	ImGui::End();
}
