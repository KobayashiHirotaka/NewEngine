#include "Player.h"
#include <cassert>
#include <numbers>

void Player::Initialize(Model* model)
{
	assert(model);

	input_ = Input::GetInstance();

	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.rotation.y = 1.6f;
}

void Player::Update()
{
	if (input_->GetJoystickState())
	{
		//コントローラーの移動処理
	}
	else {
		bool isMove_ = false;
		float kCharacterSpeed = 0.3f;
		velocity_ = { 0.0f, 0.0f, 0.0f };

		//移動処理
		if (input_->PressKey(DIK_A))
		{
			velocity_.x = -1;

		}
		else if (input_->PressKey(DIK_D)) {
			velocity_.x = 1;
		}

		if (input_->PressKey(DIK_A) || input_->PressKey(DIK_D))
		{
			isMove_ = true;
			velocity_ = Normalize(velocity_);
			velocity_ = Multiply(kCharacterSpeed,velocity_);
		}


		if (isMove_)
		{
			// 現在の位置から移動する位置へのベクトル
			Vector3 sub = (worldTransform_.translation + velocity_) - GetLocalPosition();

			// 平行移動
			worldTransform_.translation = Add(worldTransform_.translation, velocity_);

			if (sub.z != 0.0)
			{
				destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

				if (sub.z < 0.0)
				{
					destinationAngleY_ = (sub.x >= 0.0)
						? std::numbers::pi_v<float> -destinationAngleY_
						: -std::numbers::pi_v<float> -destinationAngleY_;
				}
			}
			else {
				destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f
					: -std::numbers::pi_v<float> / 2.0f;
			}

			worldTransform_.rotation.y = LerpShortAngle(worldTransform_.rotation.y, destinationAngleY_, 0.2f);

			worldTransform_.UpdateMatrix();

		}
	}

	ImGui::Begin("Player");
	ImGui::DragFloat3("rotation", &worldTransform_.rotation.x, 0.01f, -5.0f, 5.0f, "%.3f");
	ImGui::End();

	worldTransform_.UpdateMatrix();
}

void Player::Draw(const Camera camera)
{
	model_->Draw(worldTransform_, camera);
}

Vector3 Player::GetLocalPosition()
{
	// ワールド座標を入れる関数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.translation.x;
	worldPos.y = worldTransform_.translation.y;
	worldPos.z = worldTransform_.translation.z;
	return worldPos;
}
