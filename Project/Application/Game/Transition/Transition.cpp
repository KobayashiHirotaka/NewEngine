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

void Transition::StartSceneTransition(bool& isTransitionStart, SceneManager* sceneManager, const std::string& nextScene)
{
	//Transitionの開始処理
	if (isTransitionStart)
	{
		sceneTransitionTimer_ += kDeltaTime;
		transitionColor_.w = Lerp(transitionColor_.w, kTransitionStartAlpha_, sceneTransitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w >= kTransitionStartAlpha_)
		{
			sceneManager->ChangeScene(nextScene);
			return;
		}
	}
}

void Transition::EndSceneTransition(bool& isTransitionEnd)
{
	//Transitionの終了処理
	if (!isTransitionEnd)
	{
		sceneTransitionTimer_ += kDeltaTime;
		transitionColor_.w = Lerp(transitionColor_.w, kTransitionEndAlpha_, sceneTransitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w <= kTransitionEndAlpha_)
		{
			isTransitionEnd = true;
			sceneTransitionTimer_ = 0.0f;
		}
	}
}

void Transition::RoundTransition(bool& isRoundTransition)
{
	if (isRoundTransition)
	{
		roundTransitionTimer_ --;

		//トランジション
		const float kLerpSpeed = 0.1f;
		const int kTransitionOffset = 10;

		if (roundTransitionTimer_ > kHalfkRoundTransitionTime_)
		{
			transitionColor_.w = Lerp(transitionColor_.w, kTransitionStartAlpha_, kLerpSpeed);
			transitionSprite_->SetColor(transitionColor_);
		}
		else if (roundTransitionTimer_ <= kHalfkRoundTransitionTime_ - kTransitionOffset && roundTransitionTimer_ > 0)
		{
			transitionColor_.w = Lerp(transitionColor_.w, kTransitionEndAlpha_, kLerpSpeed);
			transitionSprite_->SetColor(transitionColor_);
		}
		else if (roundTransitionTimer_ <= 0)
		{
			isRoundTransition = false;
			roundTransitionTimer_ = kRoundTransitionTime_;
		}

		//ラウンド間の初期化処理
		if (roundTransitionTimer_ == kHalfkRoundTransitionTime_)
		{
			isRoundTransitioning_ = true;
		}
	}
}