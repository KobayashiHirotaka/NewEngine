#include "Player.h"
#include <cassert>

void Player::Initialize(Model* model)
{
	assert(model);

	model_ = model;
	worldTransform_.Initialize();
}

void Player::Update()
{
	worldTransform_.UpdateMatrix();
}

void Player::Draw(const Camera camera)
{
	model_->Draw(worldTransform_, camera);
}