#include "Skydome.h"
#include <cassert>

void Skydome::Initialize()
{
	//inputのinstance
	input_ = Input::GetInstance();

	model_.reset(Model::CreateFromOBJ("resource/remakePlayer", "remakePlayer.gltf"));

	worldTransform_.Initialize();
	//worldTransform_.scale = { 0.07f,0.07f,0.07f };
}

void Skydome::Update()
{
	//worldTransform_.rotation.y += 0.001f;
	//worldTransform_.rotation.z += 0.001f;

	model_->ApplyAnimation(0);

	model_->Update();

	if (input_->PressKey(DIK_0))
	{
		float animationTime;
		animationTime = model_->GetAnimationTime();
		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, model_->GetAnimation()[0].duration);

		model_->SetAnimationTime(animationTime);

		model_->ApplyAnimation(0);
	}

	worldTransform_.UpdateMatrixEuler();
}

void Skydome::Draw(const Camera camera)
{
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