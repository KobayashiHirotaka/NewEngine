/**
 * @file DebugCamera.h
 * @brief デバッグカメラの管理(移動、回転など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Utility/Math/MyMath.h"

class DebugCamera
{
public:
	/// <summary>コンストラクタ</summary>
	DebugCamera();

	/// <summary>デストラクタ</summary>
	~DebugCamera();

	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>更新</summary>
	void Update();

	//Getter
	//カメラ
	const Camera& GetCamera() { return camera_; };

	//Setter
	//ターゲット
	void SetTarget(WorldTransform* target) { target_ = target; };

private:
	//Inputのポインタ
	Input* input_ = nullptr;

	//カメラ
	Camera camera_;

	//WorldTransform
	WorldTransform worldTransform_;

	//ターゲット
	const WorldTransform* target_ = {};

	//オフセット
	Vector3 offset_{ 0.0f,0.0f,-35.0f };

	//回転行列
	Matrix4x4 matRot_ = MakeIdentity4x4();
};

