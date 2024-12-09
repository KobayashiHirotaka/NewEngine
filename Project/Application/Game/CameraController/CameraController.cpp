/**
 * @file CameraController.cpp
 * @brief ゲーム中のカメラの制御、更新を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "CameraController.h"

void CameraController::Initialize()
{
	input_ = Input::GetInstance();

	camera_.translation_ = position_;

	camera_.UpdateMatrix();
}

void CameraController::Update(const Vector3 characterPosition1, const Vector3 characterPositon2)
{
	//2体のキャラクターの中心座標
	center_ = { (characterPosition1.x + characterPositon2.x) / 2,  ((characterPosition1.y + characterPositon2.y) + 0.8f) / 2 };
	//position_.x = center.x;
	//position_.y = center.y;

	position_.x = position_.x + (center_.x - position_.x) * cameraSpeed_.x;
	position_.y = center_.y;

	//2体のキャラクターの座標の差
	Vector3 difference = characterPosition1 - characterPositon2;
	difference.y = 0.0f;
	distance_ = Length(difference);

	if (distance_ >= kPoint_)
	{
		if (distance_ > previousDistance_)
		{
			if (position_.z <= kMax_)
			{
				position_.z = kMax_;
			}
			else
			{
				//position_.z = Lerp(position_.z, max_, cameraSpeed_.z);
				position_.z -= cameraSpeed_.z;
			}
		}
	}

	if (previousDistance_ > distance_)
	{
		if (position_.z >= kMin_)
		{
			position_.z = kMin_;
		}
		else
		{
			//position_.z = Lerp(position_.z, min_, cameraSpeed_.z);
			position_.z += cameraSpeed_.z;
		}
	}

	//前フレームの距離を記録
	previousDistance_ = distance_;

	camera_.translation_ = position_;

	camera_.UpdateMatrix();
}

void CameraController::ImGui()
{
	ImGui::Begin("CameraController");
	ImGui::SliderFloat3("WTFT", &position_.x, -50.0f, 50.0f);
	ImGui::SliderFloat3("cameraSpeed", &cameraSpeed_.x, 0.01f, 0.1f);
	//ImGui::SliderFloat("point", &point_, 1.0f, 5.0f);
	ImGui::SliderFloat("distance", &distance_, 0.0f, 10.0f);
	ImGui::SliderFloat("previousDistance", &previousDistance_, 0.0f, 10.0f);
	ImGui::End();
}
