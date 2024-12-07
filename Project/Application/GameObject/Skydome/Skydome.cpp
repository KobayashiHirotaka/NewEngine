#include "Skydome.h"
#include <cassert>

/**
 * @file Skydome.cpp
 * @brief 天球の管理(初期化、更新、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

void Skydome::Initialize()
{
	//inputのinstance
	input_ = Input::GetInstance();

	//モデルの生成
	model_.reset(Model::CreateFromOBJ("resource/skydome", "skydome.obj"));

	//worldTransformの初期化
	worldTransform_.Initialize();
	worldTransform_.scale = { 250.0f,250.0f,250.0f };
}

void Skydome::Update()
{
	//映り込みの設定
	model_->GetMaterial()->SetEnvironmentCofficient(0.0f);

	//回転
	worldTransform_.rotation.y += rotationSpeed_.y;
	worldTransform_.rotation.z += rotationSpeed_.z;

	//worldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Skydome::Draw(const Camera camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera, 0);
}

void Skydome::ImGui()
{
	ImGui::Begin("Skydome");
	ImGui::DragFloat3("WTFT", &worldTransform_.translation.x, -14.0f, 14.0f);
	ImGui::DragFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 150.0f);
	ImGui::DragFloat3("WTFS", &worldTransform_.scale.x, 0.0f, 300.0f);
	ImGui::End();
}