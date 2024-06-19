#include "Skydome.h"
#include <cassert>

void Skydome::Initialize()
{
	IGame3dObject::Initialize();
	worldTransform_.scale = { 250.0f,250.0f,250.0f };
}

void Skydome::Update()
{
	worldTransform_.rotation.y += 0.001f;
	worldTransform_.rotation.z += 0.001f;

	IGame3dObject::Update();
}

void Skydome::Draw(const Camera camera)
{
	IGame3dObject::Draw(camera);
}

void Skydome::ImGui()
{
	ImGui::Begin("Skydome");
	ImGui::DragFloat3("WTFT", &worldTransform_.translation.x, -14.0f, 14.0f);
	ImGui::DragFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 150.0f);
	ImGui::DragFloat3("WTFS", &worldTransform_.scale.x, 0.1f, 300.0f);
	ImGui::End();
}