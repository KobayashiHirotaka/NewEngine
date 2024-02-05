#include "DebugCamera.h"

DebugCamera::DebugCamera()
{
	
}

DebugCamera::~DebugCamera() 
{

}

void DebugCamera::Initialize()
{
	input_ = Input::GetInstance();

	worldTransform_.Initialize();
	/*worldTransform_.translation = { 0.0f,0.0f,0.0f };*/
}

void DebugCamera::Update()
{
	if (target_)
	{
		Vector3 offset = offset_;

		offset = TransformNormal(offset, matRot_);

		worldTransform_.translation = Add(target_->translation, offset);
	}

	Vector3 offset = offset_;

	offset = TransformNormal(offset, matRot_);

	worldTransform_.translation = offset;

	Matrix4x4 translateMatrix = MakeTranslateMatrix(worldTransform_.translation);

	worldTransform_.matWorld = Multiply(matRot_, translateMatrix);

	camera_.matView_ = Multiply(Inverse(translateMatrix), Inverse(matRot_));

	camera_.matProjection_ = MakePerspectiveFovMatrix(camera_.fov_, camera_.aspectRatio_,
		camera_.nearClip_, camera_.farClip_);

	//前後
	if (input_->GetWheel() > 0)
	{
		const float speed = 1.0f;

		Vector3 move = { 0.0f,0.0f,speed };
		
		offset_ = Add(offset_, move);
	}
	else if (input_->GetWheel() < 0)
	{
		const float speed = -1.0f;

		Vector3 move = { 0.0f,0.0f,speed };

		offset_ = Add(offset_, move);
	}

	//左右
	if (input_->PressKey(DIK_D)) 
	{
		const float speed = -0.2f;

		Vector3 move = { speed,0.0f,0.0f };

		offset_ = Add(offset_, move);
	}
	else if (input_->PressKey(DIK_A))
	{
		const float speed = 0.2f;

		Vector3 move = { speed,0.0f,0.0f };

		offset_ = Add(offset_, move);
	}

	//上下
	if (input_->PressKey(DIK_W))
	{
		const float speed = -0.2f;

		Vector3 move = { 0.0f,speed,0.0f };
	
		offset_ = Add(offset_, move);
	}
	else if (input_->PressKey(DIK_S))
	{
		const float speed = 0.2f;
	
		Vector3 move = { 0.0f,speed,0.0f };
	
		offset_ = Add(offset_, move);
	}

	//X軸回転
	if (input_->PressKey(DIK_UP))
	{
		const float speed = 0.02f;
	
		Matrix4x4 matRotDelta = MakeIdentity4x4();
		matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(speed));
	
		matRot_ = Multiply(matRotDelta, matRot_);
	}
	else if (input_->PressKey(DIK_DOWN))
	{
		const float speed = -0.02f;
	
		Matrix4x4 matRotDelta = MakeIdentity4x4();
		matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(speed));
	
		matRot_ = Multiply(matRotDelta, matRot_);
	}

	//Y軸回転
	if (input_->PressKey(DIK_LEFT))
	{

		const float speed = 0.02f;

		Matrix4x4 matRotDelta = MakeIdentity4x4();
		matRotDelta = Multiply(matRotDelta, MakeRotateYMatrix(speed));

		matRot_ = Multiply(matRotDelta, matRot_);
	}
	else if (input_->PressKey(DIK_RIGHT))
	{
		const float speed = -0.02f;
	
		Matrix4x4 matRotDelta = MakeIdentity4x4();
		matRotDelta = Multiply(matRotDelta, MakeRotateYMatrix(speed));
		matRot_ = Multiply(matRotDelta, matRot_);
	}
}