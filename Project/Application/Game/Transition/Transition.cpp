/**
 * @file Transition.cpp
 * @brief トランジションの開始、終了処理を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2025/01/02
 */

#include "Transition.h"
#include "Engine/Framework/SceneManager.h"

void Transition::Initialize()
{
	//Transition用のSpriteの生成、設定
	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(transitionTextureSize_);
}

void Transition::Draw()
{
	//Transition用Spriteの描画
	transitionSprite_->Draw();
}

void Transition::StartTransition(bool& isTransitionStart, SceneManager* sceneManager, const std::string& nextScene)
{
	//Transitionの開始処理
	if (isTransitionStart)
	{
		transitionTimer_ += kDeltaTime;
		transitionColor_.w = Lerp(transitionColor_.w, kTransitionStartAlpha_, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w >= kTransitionStartAlpha_)
		{
			sceneManager->ChangeScene(nextScene);
			return;
		}
	}
}

void Transition::EndTransition(bool& isTransitionEnd)
{
	//Transitionの終了処理
	if (!isTransitionEnd)
	{
		transitionTimer_ += kDeltaTime;
		transitionColor_.w = Lerp(transitionColor_.w, kTransitionEndAlpha_, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w <= kTransitionEndAlpha_)
		{
			isTransitionEnd = true;
			transitionTimer_ = 0.0f;
		}
	}
}