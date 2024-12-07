#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Model/IGame3dObject.h"

/**
 * @file Skydome.h
 * @brief 天球の管理(初期化、更新、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

class Skydome
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

	Vector3 rotationSpeed_ = { 0.001f,0.001f,0.001f };
};
