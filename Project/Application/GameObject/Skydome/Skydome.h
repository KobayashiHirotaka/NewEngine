/**
 * @file Skydome.h
 * @brief 天球の初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/3D/Model/IGame3dObject.h"
#include "Engine/Components/Input/Input.h"

class Skydome
{
public:
	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>更新</summary>
	void Update();

	/// <summary>描画</summary>
	void Draw(const Camera camera);

	/// <summary>ImGui</summary>
	void ImGui();

private:
	//Inputのポインタ
	Engine::Input* input_ = nullptr;

	//モデル
	std::unique_ptr<Model> model_;

	//WorldTransform
	WorldTransform worldTransform_;

	//回転速度
	Vector3 rotationSpeed_ = { 0.001f,0.001f,0.001f };

	//スケール
	Vector3 scale_ = { 250.0f,250.0f,250.0f };

	//映り込み
	const float environmentCofficient_ = 0.0f;

	//アニメーションデータ
	uint32_t animationData_ = 0;
};
