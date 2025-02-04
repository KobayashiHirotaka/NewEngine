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
	//WorldTransformの初期化
	worldTransform_.Initialize();
}

void BackGround::Update()
{
	//映り込みの設定
	model_->GetMaterial()->SetEnvironmentCofficient(0.0f);

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void BackGround::Draw(const Camera& camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera, 0);
}

void BackGround::ImGui()
{

}

void BackGround::OnCollision(Collider*)
{

}