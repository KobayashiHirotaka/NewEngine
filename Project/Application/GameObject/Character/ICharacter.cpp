#include "ICharacter.h"

void ICharacter::Initialize(const std::vector<Model*>& models)
{
	models_ = models;
	worldTransform_.Initialize();
}

void ICharacter::Update()
{
	worldTransform_.UpdateMatrixEuler();
}

void ICharacter::Draw(const Camera& camera)
{
	for (Model* model : models_)
	{
		model->Draw(worldTransform_, camera);
	}
}
