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

	/// <summary>SceneTransitionの開始処理</summary>
	void StartSceneTransition(bool& isTransitionStart, SceneManager* sceneManager, const std::string& nextScene);

	/// <summary>SceneTransitionの終了処理</summary>
	void EndSceneTransition(bool& isTransitionEnd);

	/// <summary>RoundTransitionの処理</summary>
	void RoundTransition(bool &isRoundTransition);

	//Getter
	bool GetIsRoundTransitioning() { return isRoundTransitioning_; };

	//Setter
	void SetIsRoundTransitioning(bool isRoundTransitioning) { isRoundTransitioning_ = isRoundTransitioning; };

private:
	//Sprite関係
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	uint32_t transitionTextureHandle_ = 0;
	Vector2 transitionTextureSize_ = { 1280.0f,720.0f };
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };

	//Transition用の時間
	static const int kSceneTransitionTime = 60;
	const float kDeltaTime = 1.0f / 60.0f;
	float sceneTransitionTimer_ = 0;

	//Transition開始時のアルファ値
	const float kTransitionStartAlpha_ = 1.0f;

	//Transition終了時のアルファ値
	const float kTransitionEndAlpha_ = 0.0f;

	//RoundTransition用の時間
	const int kRoundTransitionTime_ = 150;
	const int kHalfkRoundTransitionTime_ = kRoundTransitionTime_ / 2;
	int roundTransitionTimer_ = kRoundTransitionTime_;

	//RoundTranisiton中かどうか
	bool isRoundTransitioning_ = false;
};

