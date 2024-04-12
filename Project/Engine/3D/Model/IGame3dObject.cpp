#include "IGame3dObject.h"

void IGame3dObject::Initialize() 
{
	worldTransform_.Initialize();
}

void IGame3dObject::Update()
{
	worldTransform_.UpdateMatrix(RotationType::Euler);
}

void IGame3dObject::Draw(const Camera& camera)
{
	if (model_ != nullptr) 
	{
		model_->Draw(worldTransform_, camera);
	}
}