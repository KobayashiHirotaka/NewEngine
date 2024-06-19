#include "IGame3dObject.h"

void IGame3dObject::Initialize()
{
	worldTransform_.Initialize();
}

void IGame3dObject::Update()
{
	worldTransform_.UpdateMatrixEuler();
}

void IGame3dObject::Draw(Model* model, const Camera camera)
{
	if (model != nullptr)
	{
		model->Draw(worldTransform_, camera, 0);
	}
}