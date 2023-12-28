#include "Skydome.h"
#include <cassert>

void Skydome::Initialize(Model* model)
{
	assert(model);

	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.scale = { 250.0f,250.0f,250.0f };
}

void Skydome::Update()
{
	worldTransform_.rotation.y += 0.001f;
	worldTransform_.rotation.z += 0.001f;
	worldTransform_.UpdateMatrix();
}

void Skydome::Draw(const Camera camera)
{
	model_->Draw(worldTransform_, camera);
}