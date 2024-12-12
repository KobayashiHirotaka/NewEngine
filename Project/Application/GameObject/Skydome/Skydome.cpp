/**
 * @file Skydome.cpp
 * @brief 天球の初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Skydome.h"
#include <cassert>

void Skydome::Initialize()
{
	//Inputのインスタンスの取得
	input_ = Input::GetInstance();

	//モデルの生成
	model_.reset(Model::CreateFromOBJ("resource/skydome", "skydome.obj"));

	//WorldTransformの初期化
	worldTransform_.Initialize();
	worldTransform_.scale = scale_;
}

void Skydome::Update()
{
	//映り込みの設定
	model_->GetMaterial()->SetEnvironmentCofficient(environmentCofficient_);

	//回転
	worldTransform_.rotation.y += rotationSpeed_.y;
	worldTransform_.rotation.z += rotationSpeed_.z;

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Skydome::Draw(const Camera camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera, animationData_);
}

void Skydome::ImGui()
{

}