#include "BackGround.h"
#include <cassert>

void BackGround::Initialize()
{
	//inputのinstance
	input_ = Input::GetInstance();

	//メインの背景
	model_[0].reset(Model::CreateFromOBJ("resource/BackStage", "wall.gltf"));
	model_[1].reset(Model::CreateFromOBJ("resource/BackStage", "wall.gltf"));
	model_[2].reset(Model::CreateFromOBJ("resource/BackStage", "wall.gltf"));
	model_[3].reset(Model::CreateFromOBJ("resource/BackStage", "wall.gltf"));
	model_[4].reset(Model::CreateFromOBJ("resource/BackStage", "wall.gltf"));
	model_[5].reset(Model::CreateFromOBJ("resource/BackStage", "wall.gltf"));
	model_[6].reset(Model::CreateFromOBJ("resource/BackStage", "wall.gltf"));
	model_[7].reset(Model::CreateFromOBJ("resource/BackStage", "wall.gltf"));
	model_[8].reset(Model::CreateFromOBJ("resource/BackStage", "testHouse.gltf"));

	worldTransform_[0].Initialize();
	worldTransform_[0].translation = { 6.8f,0.0f,13.0f };
	worldTransform_[0].rotation.y = 3.8f;
	worldTransform_[0].scale = { 4.0f,4.0f,1.8f };

	worldTransform_[1].Initialize();
	worldTransform_[1].translation = { -6.8f,0.0f,13.0f };
	worldTransform_[1].rotation.y = 2.5f;
	worldTransform_[1].scale = { 2.0f,2.0f,1.8f };

	worldTransform_[2].Initialize();
	worldTransform_[2].translation = { 0.0f,0.0f,17.0f };
	worldTransform_[2].rotation.y = 3.2f;
	worldTransform_[2].scale = { 2.0f,2.0f,1.8f };

	worldTransform_[3].Initialize();
	worldTransform_[3].translation = { -18.0f,-0.4f,70.0f };
	worldTransform_[3].scale = { 1.5f,1.5f,1.5f };

	worldTransform_[4].Initialize();
	worldTransform_[4].translation = { -30.0f,-0.4f,70.0f };
	worldTransform_[4].scale = { 1.7f,1.7f,1.7f };

	worldTransform_[5].Initialize();
	worldTransform_[5].translation = { -8.0f,-0.4f,17.0f };
	worldTransform_[5].scale = { 1.5f,6.0f,1.8f };

	worldTransform_[6].Initialize();
	worldTransform_[6].translation = { 15.0f,11.0f,75.0f };
	worldTransform_[6].scale = { 1.3f,1.3f,1.3f };

	worldTransform_[7].Initialize();
	worldTransform_[7].translation = { -17.0f,15.5f,90.0f };
	worldTransform_[7].scale = { 1.5f,1.5f,1.5f };

	worldTransform_[8].Initialize();
	worldTransform_[8].translation = { 0.0f,-0.4f,70.0f };
	worldTransform_[8].scale = { 1.8f,1.8f,1.8f };

	for (int i = 0; i < 9; i++)
	{
		model_[i]->GetMaterial()->SetEnvironmentCofficient(0.0f);
	}
}

void BackGround::Update()
{
	for (int i = 0; i < 9; i++)
	{
		worldTransform_[i].UpdateMatrixEuler();
	}
}

void BackGround::Draw(const Camera camera)
{
	if (stateNum_ == 0)
	{
		for (int i = 0; i < 8; i++)
		{
			model_[i]->Draw(worldTransform_[i], camera, 0);
		}
	}

	if (stateNum_ == 1)
	{
		for (int i = 1; i < 9; i++)
		{
			model_[i]->Draw(worldTransform_[i], camera, 0);
		}
	}
}

void BackGround::ImGui()
{
	ImGui::Begin("BackGround");
	ImGui::SliderFloat3("WTFT", &worldTransform_[0].translation.x, -40.0f, 40.0f);
	ImGui::SliderFloat3("WTFR", &worldTransform_[0].rotation.x, 0.0f, 150.0f);
	ImGui::SliderFloat3("WTFS", &worldTransform_[0].scale.x, 0.0f, 300.0f);
	ImGui::End();
}
