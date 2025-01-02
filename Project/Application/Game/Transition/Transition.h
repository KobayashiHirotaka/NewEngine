/**
 * @file Transition.h
 * @brief トランジションの開始、終了処理を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2025/01/02
 */

#pragma once
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/2D/Sprite/Sprite.h"

class SceneManager;

class Transition
{
public:
	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>描画</summary>
	void Draw();

	/// <summary>Transitionの開始処理</summary>
	void StartTransition(bool& isTransitionStart, SceneManager* sceneManager, const std::string& nextScene);

	/// <summary>Transitionの終了処理</summary>
	void EndTransition(bool& isTransitionEnd);

private:
	//Sprite関係
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	uint32_t transitionTextureHandle_ = 0;
	Vector2 transitionTextureSize_ = { 1280.0f,720.0f };
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };

	//Transition用の時間
	static const int kTransitionTime = 60;
	const float kDeltaTime = 1.0f / kTransitionTime;
	float transitionTimer_ = 0;

	//Transition開始時のアルファ値
	const float kTransitionStartAlpha_ = 1.0f;

	//Transition終了時のアルファ値
	const float kTransitionEndAlpha_ = 0.0f;
};

