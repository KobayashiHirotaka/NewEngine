/**
 * @file Ground.cpp
 * @brief 地面の初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Ground.h"
#include <cassert>

void Ground::Initialize()
{
	//WorldTransformの初期化
	worldTransform_.Initialize();
}

void Ground::Update()
{
	//映り込みの設定
	model_->GetMaterial()->SetEnvironmentCofficient(0.0f);

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Ground::Draw(const Camera& camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera, 0);
}

void Ground::ImGui(const char* title)
{

}
