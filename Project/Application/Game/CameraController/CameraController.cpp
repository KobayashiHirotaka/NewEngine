/**
 * @file CameraController.cpp
 * @brief ゲーム中のカメラの制御、更新を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "CameraController.h"

void CameraController::Initialize()
{
	//Inputのインスタンスの取得
	input_ = Input::GetInstance();

	//カメラの座標を設定
	camera_.translation_ = position_;

	//カメラの更新
	camera_.UpdateMatrix();
}

void CameraController::Update(const Vector3 characterPosition1, const Vector3 characterPositon2)
{
	//2体のキャラクターの中心座標
	center_ = { (characterPosition1.x + characterPositon2.x) / 2,  ((characterPosition1.y + characterPositon2.y) + 0.8f) / 2 };

	//カメラの座標(X,Y)を常に中心にする
	position_.x = position_.x + (center_.x - position_.x) * cameraSpeed_.x;
	position_.y = center_.y;

	//2体のキャラクターの座標の差
	Vector3 difference = characterPosition1 - characterPositon2;
	difference.y = 0.0f;
	distance_ = Length(difference);

	//カメラのZ座標を動かす
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
			position_.z += cameraSpeed_.z;
		}
	}

	//前フレームの距離を記録
	previousDistance_ = distance_;

	//カメラの座標を設定
	camera_.translation_ = position_;

	//カメラの更新
	camera_.UpdateMatrix();
}

void CameraController::ImGui()
{

}

void CameraController::StartFinisherCamera(Direction direction, float positionX)
{
	if (direction == Direction::Right)
	{
		camera_.translation_.x = Lerp(camera_.translation_.x, positionX + kCameraFinisherOffsetY_, kCameraLerpSpeed_);
		camera_.rotation_.y = Lerp(camera_.rotation_.y, -kCameraFinisherRotationY_, kCameraLerpSpeed_);
	}
	else
	{
		camera_.translation_.x = Lerp(camera_.translation_.x, positionX - kCameraFinisherOffsetY_, kCameraLerpSpeed_);
		camera_.rotation_.y = Lerp(camera_.rotation_.y, kCameraFinisherRotationY_, kCameraLerpSpeed_);
	}

	camera_.UpdateMatrix();
}

void CameraController::EndFinisherCamera(Direction direction, bool& isFinisherEnd)
{
	if (direction == Direction::Right)
	{
		camera_.translation_.x = Lerp(camera_.translation_.x, center_.x - kCameraEndCorrectionY_, kCameraLerpSpeed_);
		camera_.rotation_.y = Lerp(camera_.rotation_.y, kCameraEndCorrectionY_, kCameraSmallLerpSpeed_);

		if (camera_.translation_.x <= center_.x && camera_.rotation_.y >= 0.0f)
		{
			camera_.translation_.x = center_.x;
			camera_.rotation_.y = 0.0f;
			isFinisherEnd = false;
		}
	}
	else
	{
		camera_.translation_.x = Lerp(camera_.translation_.x, center_.x + kCameraEndCorrectionY_, kCameraLerpSpeed_);
		camera_.rotation_.y = Lerp(camera_.rotation_.y, -kCameraEndCorrectionY_, kCameraSmallLerpSpeed_);

		if (camera_.translation_.x >= center_.x && camera_.rotation_.y <= 0.0f)
		{
			camera_.translation_.x = center_.x;
			camera_.rotation_.y = 0.0f;
			isFinisherEnd = false;
		}
	}

	camera_.UpdateMatrix();
}