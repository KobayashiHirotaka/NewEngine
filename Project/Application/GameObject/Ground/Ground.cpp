#include "Ground.h"
#include <cassert>

void Ground::Initialize()
{
	//worldTransformの初期化
	worldTransform_.Initialize();
}

void Ground::Update()
{
	//映り込みの設定
	model_->GetMaterial()->SetEnvironmentCofficient(0.0f);

	//model_->GetMaterial()->ImGui();

	//worldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Ground::Draw(const Camera& camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera, 0);
}

void Ground::ImGui(const char* title)
{
	ImGui::Begin(title);
	ImGui::DragFloat3("WTFT", &worldTransform_.translation.x, -14.0f, 14.0f);
	ImGui::DragFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 150.0f);
	ImGui::DragFloat3("WTFS", &worldTransform_.scale.x, 0.0f, 300.0f);
	ImGui::End();
}
