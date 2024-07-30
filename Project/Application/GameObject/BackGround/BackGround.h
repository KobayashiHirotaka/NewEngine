#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Model/IGame3dObject.h"

class BackGround
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera camera);

	void ImGui();

private:
	Input* input_ = nullptr;

	std::unique_ptr<Model> model_;

	WorldTransform worldTransform_;

	//スケールアニメーション用の変数
	float scaleAnimationTime_ = 0.0f;
	const float scaleSpeed_ = 2.0f; 
	const float minScale_ = 0.7f;
	const float maxScale_ = 1.2f;
};
