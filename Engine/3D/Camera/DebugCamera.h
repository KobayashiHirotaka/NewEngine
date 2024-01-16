//#pragma once
//#include "Engine/Base/ImGuiManager/ImGuiManager.h"
//#include "Engine/3D/WorldTransform/WorldTransform.h"
//#include "Engine/3D/Camera/Camera.h"
//#include "Engine/Components/Input/Input.h"
//#include "Engine/Utility/Math/MyMath.h"
//#include "Engine/Utility/Structs/Vector3.h"
//
//class DebugCamera
//{
//public:
//	DebugCamera();
//
//	~DebugCamera();
//
//	void Initialize(int32_t kClientWidth, int32_t kClientHeight);
//
//	void Update();
//
//	const Camera& GetCamera() { return camera_; };
//
//	void SetTarget(const WorldTransform* target) { target_ = target; };
//
//#ifdef _DEBUG
//	void DebugCameraMove();
//
//	void SwitchDebugCamera(bool Flag)
//	{
//		DebucCameraFlag = Flag;
//	}
//#endif // DEBUG
//
//private:
//	int32_t kClientWidth_;
//	int32_t kClientHeight_;
//
//	Input* input_ = nullptr;
//
//	//ワールドトランスフォーム
//	WorldTransform worldTransform_{};
//
//	//ビュープロジェクション
//	Camera camera_{};
//
//	//オフセット
//	Vector3 offset_ = { 0.0f,0.0f,-50.0f };
//
//	//累積回転行列
//	Matrix4x4 matRot_ = MakeIdentity4x4();
//
//	//追従対象
//	const WorldTransform* target_ = nullptr;
//
//#ifdef _DEBUG
//	bool DebucCameraFlag = false;
//
//	const float speed = 0.03f;
//#endif // DEBUG
//};
