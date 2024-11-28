#include "CameraController.h"

void CameraController::Initialize()
{
	input_ = Input::GetInstance();

	camera_.translation_ = position_;

	camera_.UpdateMatrix();
}

void CameraController::Update(const Vector3 characterPosition1, const Vector3 characterPositon2, const Direction direction)
{
#ifdef _ADJUSTMENT

	if (input_->PressKey(DIK_W))
	{
		position_.z += cameraSpeed_;
	}

	if (input_->PressKey(DIK_S))
	{
		position_.z -= cameraSpeed_;
	}

	if (input_->PressKey(DIK_D))
	{
		position_.x += cameraSpeed_;
	}

	if (input_->PressKey(DIK_A))
	{
		position_.x -= cameraSpeed_;
	}

#endif

	//2体のキャラクターの中心座標
	float centerX = (characterPosition1.x + characterPositon2.x) / 2;
	position_.x = centerX;

	//2体のキャラクターの座標の差
	Vector3 difference = characterPosition1 - characterPositon2;
	float distance = Length(difference);

	if (input_->GetJoystickState())
	{
		if (distance >= point_)
		{
			if (direction == Direction::Right && input_->GetLeftStickX() <= -input_->GetDeadZone() ||
				direction == Direction::Left && input_->GetLeftStickX() >= input_->GetDeadZone())
			{
				if (position_.z <= max_)
				{
					position_.z = max_;
				}
				else
				{
					//position_.z = Lerp(position_.z, max_, cameraSpeed_);
					position_.z -= cameraSpeed_;
				}
			}
		}

		if (direction == Direction::Right && input_->GetLeftStickX() >= input_->GetDeadZone() ||
			direction == Direction::Left && input_->GetLeftStickX() <= -input_->GetDeadZone())
		{
			if (position_.z >= min_)
			{
				position_.z = min_;
			}
			else
			{
				//position_.z = Lerp(position_.z, min_, cameraSpeed_);
				position_.z += cameraSpeed_;
			}
		}
	}

	camera_.translation_ = position_;

	camera_.UpdateMatrix();
}

void CameraController::ImGui()
{
	ImGui::Begin("CameraController");
	ImGui::SliderFloat3("WTFT", &position_.x, -50.0f, 50.0f);
	ImGui::SliderFloat("cameraSpeed", &cameraSpeed_, 0.01f, 0.1f);
	ImGui::SliderFloat("point", &point_, 1.0f, 5.0f);
	ImGui::End();
}
