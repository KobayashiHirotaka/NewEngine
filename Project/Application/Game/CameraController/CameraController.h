/**
 * @file CameraController.h
 * @brief ゲーム中のカメラの制御、更新を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Application/GameObject/Character/Direction.h"

class CameraController
{
public:
	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>更新</summary>
	void Update(const Vector3 characterPosition1, const Vector3 characterPositon2);

	/// <summary>ImGui</summary>
	void ImGui();

	/// <summary>必殺技のカメラ演出の開始処理</summary>
	void StartFinisherCamera(Direction direction, float positionX);

	/// <summary>必殺技のカメラ演出の終了処理</summary>
	void EndFinisherCamera(Direction direction, bool& isFinisherEnd);

	//カメラのGetter
	Camera& GetCamera() { return camera_; };

	//中心のGetter
	Vector2 GetCenter() { return center_; };

private:
	//Inputのポインタ
	Input* input_ = nullptr;

	//カメラ
	Camera camera_;

	//位置
	const float kCameraOffsetY_ = 1.0f;
	Vector3 position_ = { 0.0f, kCameraOffsetY_, -5.0f };

	//カメラの速さ
	Vector3 cameraSpeed_ = { 1.5f, 0.5f, 0.03f };

	//中心
	Vector2 center_ = { 0.0f,0.0f };

	//カメラが移動をはじめるポイント
	const float kPoint_ = 2.8f;

	//カメラのZ座標の最小値
	const float kMin_ = -5.0f;

	//カメラのZ座標の最大値
	const float kMax_ = -8.0f;

	//距離
	float distance_ = 0.0f;

	//前フレームの距離
	float previousDistance_ = 0.0f;

	//必殺技時のカメラ演出用のパラメータ
    //プレイヤーの位置からのカメラのオフセット
	const float kCameraFinisherOffsetY_ = 4.0f;

	//カメラ移動と回転の補間速度
	const float kCameraLerpSpeed_ = 0.2f;

	//必殺技発動時のカメラ回転角
	const float kCameraFinisherRotationY_ = 0.7f;

	//必殺技終了時のカメラ微調整オフセット
	const float kCameraEndCorrectionY_ = 0.1f;

	//微調整用補間速度
	const float kCameraSmallLerpSpeed_ = 0.1f;

	//微小な回転補正角度
	const float kCameraSmallRotationCorrection_ = 0.1f;
};
