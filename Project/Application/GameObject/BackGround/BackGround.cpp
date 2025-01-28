/**
 * @file BackGround.cpp
 * @brief 背景の初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "BackGround.h"
#include <cassert>

void BackGround::Initialize()
{
	//Inputのインスタンスの取得
	input_ = Engine::Input::GetInstance();

	//背景オブジェクトの生成
	model_[0].reset(Model::CreateFromOBJ("Resource/BackStage", "Wall.gltf"));
	model_[1].reset(Model::CreateFromOBJ("Resource/BackStage", "Wall.gltf"));
	model_[2].reset(Model::CreateFromOBJ("Resource/BackStage", "HighBill.gltf"));
	model_[3].reset(Model::CreateFromOBJ("Resource/BackStage", "LowBill.gltf"));
	model_[4].reset(Model::CreateFromOBJ("Resource/BackStage", "Wall.gltf"));
	model_[5].reset(Model::CreateFromOBJ("Resource/BackStage", "LowBill.gltf"));
	model_[6].reset(Model::CreateFromOBJ("Resource/BackStage", "LowBill.gltf"));
	model_[7].reset(Model::CreateFromOBJ("Resource/BackStage", "HighBill.gltf"));

	//各背景オブジェクトのWorldTransformの初期化
	for (int i = 0; i < kMaxBackGroundObject_; i++)
	{
		worldTransform_[i].Initialize();
		worldTransform_[i].translation = translation_[i];
		worldTransform_[i].rotation.y = rotationY_[i];
		worldTransform_[i].scale = scale_[i];
	}

	for (int i = 0; i < kMaxBackGroundObject_; i++)
	{
		model_[i]->GetMaterial()->SetEnvironmentCofficient(0.0f);
	}
}

void BackGround::Update()
{
	//各背景オブジェクトのWorldTransformの更新
	for (int i = 0; i < kMaxBackGroundObject_; i++)
	{
		worldTransform_[i].UpdateMatrixEuler();
	}
}

void BackGround::Draw(const Camera camera)
{
	//各背景オブジェクトの描画
	for (int i = 0; i < kMaxBackGroundObject_; i++)
	{
		model_[i]->Draw(worldTransform_[i], camera, 0);
	}
}

void BackGround::ImGui()
{
	
}
