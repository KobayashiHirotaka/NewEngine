#pragma once
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Utility/Math/MyMath.h"

class DebugCamera
{
public:
	DebugCamera();

	~DebugCamera();

	void Initialize();

	void Update();

	const Camera& GetCamera() { return camera_; };

	void SetTarget(WorldTransform* target) { target_ = target; };

private:
	WorldTransform worldTransform_;

	const WorldTransform* target_ = {};

	Camera camera_;

	Input* input_ = nullptr;

	Vector3 offset_{ 0.0f,0.0f,-35.0f };

	Matrix4x4 matRot_ = MakeIdentity4x4();
};

