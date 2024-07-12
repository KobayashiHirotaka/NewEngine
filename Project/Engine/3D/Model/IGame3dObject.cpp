#include "IGame3dObject.h"

void IGame3dObject::Initialize()
{
	worldTransform_.Initialize();
}

void IGame3dObject::Update()
{
	worldTransform_.UpdateMatrixEuler();
}

void IGame3dObject::Draw(const Camera& camera)
{
	
}