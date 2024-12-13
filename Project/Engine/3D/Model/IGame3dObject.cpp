/**
 * @file IGame3dObject.cpp
 * @brief 3Dオブジェクトの基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "IGame3dObject.h"

void IGame3dObject::Initialize()
{
	//WorldTransformの初期化
	worldTransform_.Initialize();

	//映り込みの設定
	model_->GetMaterial()->SetEnvironmentCofficient(0.0f);
}

void IGame3dObject::Update()
{
	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}