/**
 * @file BackGround.h
 * @brief 背景の初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/3D/Model/IGame3dObject.h"
#include "Engine/Components/Input/Input.h"

class BackGround
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

	//背景オブジェクトの数
	static const int kMaxBackGroundObject_ = 8;

	//モデル
	std::unique_ptr<Model> model_[kMaxBackGroundObject_];

	//WorldTransform
	WorldTransform worldTransform_[kMaxBackGroundObject_];

	//各背景オブジェクトのtranslation
	const Vector3 translation_[kMaxBackGroundObject_] = { { 6.8f,0.0f,13.0f }, { -7.0f,0.0f,5.0f },
		{ 0.0f,0.0f,12.3f }, { -18.0f,-0.4f,60.0f },  { -30.0f,-0.4f,6.0f }, { -5.8f,-0.2f,24.0f },
		{ 6.2f,-0.4f,55.0f }, { -7.0f,-3.0f,74.0f }
	};

	//各背景オブジェクトのrotationY
	const float rotationY_[kMaxBackGroundObject_] = { 3.8f, -0.8f, 3.2f, 0.6f, 0.0f, 2.9f, 0.0f,0.0f };

	//各背景オブジェクトのscale
	const Vector3 scale_[kMaxBackGroundObject_] = { { 5.0f,4.0f,1.8f }, { 3.0f,2.0f,1.8f },
		{ 3.0f,2.0f,2.0f }, { 5.0f,4.0f,4.0f },  { 1.7f,1.7f,1.7f }, { 3.5f,3.0f,3.0f }, 
		{ 3.2f,2.0f,1.3f }, { 3.0f,3.0f,1.5f }
	};

	//スケールアニメーション用の変数
	float scaleAnimationTime_ = 0.0f;
	const float scaleSpeed_ = 2.0f; 
	const float minScale_ = 0.7f;
	const float maxScale_ = 1.2f;

	//ステージの状態
	int stateNum_ = 0;
};
