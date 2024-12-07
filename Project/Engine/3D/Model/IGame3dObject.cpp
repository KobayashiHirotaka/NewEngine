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