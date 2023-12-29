#include "Player.h"
#include <cassert>
#include <numbers>

void Player::Initialize(const std::vector<Model*>& models)
{
	input_ = Input::GetInstance();

	ICharacter::Initialize(models);
	worldTransform_.translation = { -7.0f,0.0f,0.0f };

	worldTransformHead_.Initialize();
	worldTransform_.rotation.y = 1.7f;

	worldTransformBody_.Initialize();
	worldTransformBody_.translation = { 0.0f,1.0f,0.0f };

	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.translation.x = 0.5f;

	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.translation.x = -0.5f;

	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Player::Update()
{
	if (input_->GetJoystickState())
	{
		//コントローラーの移動処理
	}
	else {
		bool isMove_ = false;
		float kCharacterSpeed = 0.1f;
		velocity_ = { 0.0f, 0.0f, 0.0f };

		//移動処理
		if (input_->PressKey(DIK_A))
		{
			velocity_.x = -0.03f;
			worldTransform_.rotation.y = 4.6f;
		}
		else if (input_->PressKey(DIK_D)) {
			velocity_.x = 0.03f;
			worldTransform_.rotation.y = 1.7f;
		}

		if (input_->PressKey(DIK_A) || input_->PressKey(DIK_D))
		{
			isMove_ = true;
			velocity_ = Normalize(velocity_);
			velocity_ = Multiply(kCharacterSpeed,velocity_);
		}


		if (isMove_)
		{
			// 平行移動
			worldTransform_.translation = Add(worldTransform_.translation, velocity_);

			worldTransform_.UpdateMatrix();

		}
	}

	ImGui::Begin("Player");
	ImGui::DragFloat3("rotation", &worldTransform_.rotation.x, 0.01f, -5.0f, 5.0f, "%.3f");
	ImGui::End();

	worldTransform_.UpdateMatrix();

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Player::Draw(const Camera& camera)
{
	/*model_->Draw(worldTransform_, camera);*/

	models_[kModelIndexBody]->Draw(worldTransformBody_, camera);
	models_[kModelIndexHead]->Draw(worldTransformHead_, camera);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, camera);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, camera);
}

void Player::OnCollision(Collider* collider)
{

}

Vector3 Player::GetWorldPosition()
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld.m[3][0];
	pos.y = worldTransform_.matWorld.m[3][1];
	pos.z = worldTransform_.matWorld.m[3][2];
	return pos;
}


