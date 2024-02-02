#include "Player.h"

void Player::Initialize()
{
	input_ = Input::GetInstance();

	IGame3dObject::Initialize();
	worldTransform_.translation.y = 3.0f;
}

void Player::Update()
{
	IGame3dObject::Update();
}

void Player::Draw(const Camera& camera)
{
	IGame3dObject::Draw(camera);
}
