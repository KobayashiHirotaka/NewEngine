#include "BackGround.h"
#include <cassert>

/**
 * @file BackGround.cpp
 * @brief 背景の管理(初期化、更新、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

void BackGround::Initialize()
{
	//inputのinstance
	input_ = Input::GetInstance();

	//メインの背景
	model_[0].reset(Model::CreateFromOBJ("resource/BackStage", "wall.gltf"));
	model_[1].reset(Model::CreateFromOBJ("resource/BackStage", "wall.gltf"));
	model_[2].reset(Model::CreateFromOBJ("resource/BackStage", "highBill.gltf"));
	model_[3].reset(Model::CreateFromOBJ("resource/BackStage", "lowBill.gltf"));
	model_[4].reset(Model::CreateFromOBJ("resource/BackStage", "wall.gltf"));
	model_[5].reset(Model::CreateFromOBJ("resource/BackStage", "lowBill.gltf"));
	model_[6].reset(Model::CreateFromOBJ("resource/BackStage", "lowBill.gltf"));
	model_[7].reset(Model::CreateFromOBJ("resource/BackStage", "highBill.gltf"));
	model_[8].reset(Model::CreateFromOBJ("resource/BackStage", "testHouse.gltf"));

	worldTransform_[0].Initialize();
	worldTransform_[0].translation = { 6.8f,0.0f,13.0f };
	worldTransform_[0].rotation.y = 3.8f;
	worldTransform_[0].scale = { 5.0f,4.0f,1.8f };

	worldTransform_[1].Initialize();
	worldTransform_[1].translation = { -7.0f,0.0f,5.0f };
	worldTransform_[1].rotation.y = -0.8f;
	worldTransform_[1].scale = { 3.0f,2.0f,1.8f };

	worldTransform_[2].Initialize();
	worldTransform_[2].translation = { 0.0f,0.0f,12.3f };
	worldTransform_[2].rotation.y = 3.2f;
	worldTransform_[2].scale = { 3.0f,2.0f,2.0f };

	worldTransform_[3].Initialize();
	worldTransform_[3].translation = { -18.0f,-0.4f,60.0f };
	worldTransform_[3].rotation.y = 0.6f;
	worldTransform_[3].scale = { 5.0f,4.0f,4.0f };

	worldTransform_[4].Initialize();
	worldTransform_[4].translation = { -30.0f,-0.4f,6.0f };
	worldTransform_[4].scale = { 1.7f,1.7f,1.7f };

	worldTransform_[5].Initialize();
	worldTransform_[5].translation = { -5.8f,-0.2f,24.0f };
	worldTransform_[5].rotation.y = 2.9f;
	worldTransform_[5].scale = { 3.5f,3.0f,3.0f };

	worldTransform_[6].Initialize();
	worldTransform_[6].translation = { 6.2f,-0.4f,55.0f };
	worldTransform_[6].scale = { 3.2f,2.0f,1.3f };

	worldTransform_[7].Initialize();
	worldTransform_[7].translation = { -7.0f,-3.0f,74.0f };
	worldTransform_[7].scale = { 3.0f,3.0f,1.5f };

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
	//ImGui();

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
	ImGui::SliderFloat3("WTFT0", &worldTransform_[0].translation.x, -40.0f, 40.0f);
	ImGui::SliderFloat3("WTFR0", &worldTransform_[0].rotation.x, 0.0f, 150.0f);
	ImGui::SliderFloat3("WTFS0", &worldTransform_[0].scale.x, 0.0f, 300.0f);

	ImGui::SliderFloat3("WTFT1", &worldTransform_[1].translation.x, -40.0f, 40.0f);
	ImGui::SliderFloat3("WTFR1", &worldTransform_[1].rotation.x, 0.0f, 150.0f);
	ImGui::SliderFloat3("WTFS1", &worldTransform_[1].scale.x, 0.0f, 300.0f);

	ImGui::SliderFloat3("WTFT2", &worldTransform_[2].translation.x, -40.0f, 40.0f);
	ImGui::SliderFloat3("WTFR2", &worldTransform_[2].rotation.x, 0.0f, 150.0f);
	ImGui::SliderFloat3("WTFS2", &worldTransform_[2].scale.x, 0.0f, 300.0f);

	ImGui::SliderFloat3("WTFT3", &worldTransform_[3].translation.x, -40.0f, 40.0f);
	ImGui::SliderFloat3("WTFR3", &worldTransform_[3].rotation.x, 0.0f, 150.0f);
	ImGui::SliderFloat3("WTFS3", &worldTransform_[3].scale.x, 0.0f, 300.0f);

	ImGui::SliderFloat3("WTFT4", &worldTransform_[4].translation.x, -40.0f, 40.0f);
	ImGui::SliderFloat3("WTFR4", &worldTransform_[4].rotation.x, 0.0f, 150.0f);
	ImGui::SliderFloat3("WTFS4", &worldTransform_[4].scale.x, 0.0f, 300.0f);

	ImGui::SliderFloat3("WTFT5", &worldTransform_[5].translation.x, -40.0f, 40.0f);
	ImGui::SliderFloat3("WTFR5", &worldTransform_[5].rotation.x, 0.0f, 150.0f);
	ImGui::SliderFloat3("WTFS5", &worldTransform_[5].scale.x, 0.0f, 300.0f);

	ImGui::SliderFloat3("WTFT6", &worldTransform_[6].translation.x, -40.0f, 40.0f);
	ImGui::SliderFloat3("WTFR6", &worldTransform_[6].rotation.x, 0.0f, 150.0f);
	ImGui::SliderFloat3("WTFS6", &worldTransform_[6].scale.x, 0.0f, 300.0f);

	ImGui::SliderFloat3("WTFT7", &worldTransform_[7].translation.x, -40.0f, 40.0f);
	ImGui::SliderFloat3("WTFR7", &worldTransform_[7].rotation.x, 0.0f, 150.0f);
	ImGui::SliderFloat3("WTFS7", &worldTransform_[7].scale.x, 0.0f, 300.0f);
	ImGui::End();
}
