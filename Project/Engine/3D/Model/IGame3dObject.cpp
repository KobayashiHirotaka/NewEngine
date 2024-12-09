/**
 * @file IGame3dObject.cpp
 * @brief 3Dオブジェクトの基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "IGame3dObject.h"

void IGame3dObject::Initialize()
{
	worldTransform_.Initialize();

	model_->GetMaterial()->SetEnvironmentCofficient(0.0f);
}

void IGame3dObject::Update()
{
	worldTransform_.UpdateMatrixEuler();
}