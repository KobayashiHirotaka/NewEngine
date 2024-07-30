#include "BackGround.h"
#include <cassert>

void BackGround::Initialize()
{
	//inputのinstance
	input_ = Input::GetInstance();

	model_.reset(Model::CreateFromOBJ("resource/models", "backGroundCube.gltf"));

	worldTransform_.Initialize();
	worldTransform_.translation.y = 2.3f;
	worldTransform_.translation.z = 5.0f;
}

void BackGround::Update()
{
	//アニメーション
	float animationTime = 0.0f;

	animationTime = model_->GetAnimationTime();

	animationTime += 1.0f / 60.0f;

	animationTime = std::fmod(animationTime, model_->GetAnimation()[0].duration);

	model_->SetAnimationTime(animationTime);
	model_->ApplyAnimation(0);

	//スケールアニメーション
	scaleAnimationTime_ += 1.0f / 60.0f;
	float scale = minScale_ + (maxScale_ - minScale_) * (0.5f * std::sin(scaleSpeed_ * scaleAnimationTime_) + 0.5f);
	worldTransform_.scale = { scale, scale, scale };

	worldTransform_.UpdateMatrixEuler();
}

void BackGround::Draw(const Camera camera)
{
	model_->Draw(worldTransform_, camera, 0);
}

void BackGround::ImGui()
{
	ImGui::Begin("BackGround");
	ImGui::DragFloat3("WTFT", &worldTransform_.translation.x, -14.0f, 14.0f);
	ImGui::DragFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 150.0f);
	ImGui::DragFloat3("WTFS", &worldTransform_.scale.x, 0.0f, 300.0f);
	ImGui::End();
}
