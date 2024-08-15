#include "BackGround.h"
#include <cassert>

void BackGround::Initialize()
{
	//inputのinstance
	input_ = Input::GetInstance();

	model_[0].reset(Model::CreateFromOBJ("resource/BackStage", "House.gltf"));
	model_[1].reset(Model::CreateFromOBJ("resource/BackStage", "Grass1.gltf"));
	model_[2].reset(Model::CreateFromOBJ("resource/BackStage", "Grass2.gltf"));
	model_[3].reset(Model::CreateFromOBJ("resource/BackStage", "Grass3.gltf"));
	model_[4].reset(Model::CreateFromOBJ("resource/BackStage", "Grass4.gltf"));
	model_[5].reset(Model::CreateFromOBJ("resource/BackStage", "Tree.gltf"));
	model_[6].reset(Model::CreateFromOBJ("resource/BackStage", "Planet1.gltf"));
	model_[7].reset(Model::CreateFromOBJ("resource/BackStage", "Planet2.gltf"));

	worldTransform_[0].Initialize();
	worldTransform_[0].translation = { 0.0f,-0.4f,70.0f };
	worldTransform_[0].scale = { 1.3f,1.3f,1.3f };

	worldTransform_[1].Initialize();
	worldTransform_[1].translation = { 18.0f,-0.4f,70.0f };
	worldTransform_[1].scale = { 2.0f,2.0f,2.0f };

	worldTransform_[2].Initialize();
	worldTransform_[2].translation = { 25.0f,-0.4f,70.0f };
	worldTransform_[2].scale = { 2.0f,2.0f,2.0f };

	worldTransform_[3].Initialize();
	worldTransform_[3].translation = { -18.0f,-0.4f,70.0f };
	worldTransform_[3].scale = { 1.5f,1.5f,1.5f };

	worldTransform_[4].Initialize();
	worldTransform_[4].translation = { -30.0f,-0.4f,70.0f };
	worldTransform_[4].scale = { 1.7f,1.7f,1.7f };

	worldTransform_[5].Initialize();
	worldTransform_[5].translation = { -25.0f,-0.4f,70.0f };
	worldTransform_[5].scale = { 1.3f,1.3f,1.3f };

	worldTransform_[6].Initialize();
	worldTransform_[6].translation = { 12.0f,9.0f,70.0f };
	worldTransform_[6].scale = { 1.3f,1.3f,1.3f };

	worldTransform_[7].Initialize();
	worldTransform_[7].translation = { -12.0f,6.5f,70.0f };
	worldTransform_[7].scale = { 1.3f,1.3f,1.3f };

	for (int i = 0; i < 8; i++)
	{
		model_[i]->GetMaterial()->SetEnvironmentCofficient(0.3f);
	}
}

void BackGround::Update()
{
	float animationTime = 0.0f;
	animationTime = model_[6]->GetAnimationTime();

	animationTime += 1.0f / 60.0f;

	animationTime = std::fmod(animationTime, model_[6]->GetAnimation()[0].duration);

	model_[6]->SetAnimationTime(animationTime);
	model_[6]->ApplyAnimation(0);


	for (int i = 0; i < 8; i++)
	{
		worldTransform_[i].UpdateMatrixEuler();
	}
}

void BackGround::Draw(const Camera camera)
{
	for (int i = 0; i < 8; i++)
	{
		model_[i]->Draw(worldTransform_[i], camera, 0);
	}
}

void BackGround::ImGui()
{
	ImGui::Begin("BackGround");
	ImGui::SliderFloat3("WTFT", &worldTransform_[5].translation.x, -40.0f, 40.0f);
	ImGui::SliderFloat3("WTFR", &worldTransform_[5].rotation.x, 0.0f, 150.0f);
	ImGui::SliderFloat3("WTFS", &worldTransform_[5].scale.x, 0.0f, 300.0f);
	ImGui::End();
}
