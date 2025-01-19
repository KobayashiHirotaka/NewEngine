/**
 * @file GameTitleSceneUI.cpp
 * @brief タイトルシーンのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2024/01/15
 */

#include "GameTitleSceneUI.h"

void GameTitleSceneUI::Initialize()
{
	//UIの初期化
	//タイトル(モノクロファイター)のSprite
	titleTextureHandle_ = TextureManager::LoadTexture("Resource/Images/Title.png");
	titleSprite_.reset(Sprite::Create(titleTextureHandle_, { titleSpritePosition_.x, titleSpritePosition_.y }));

	//タイトル(操作用)のSprite
	titleUITextureHandle_ = TextureManager::LoadTexture("Resource/Images/TitleUI.png");
	titleUISprite_.reset(Sprite::Create(titleUITextureHandle_, { 0.0f,0.0f }));
}

void GameTitleSceneUI::Update()
{
	//タイトルの文字を動かす
	AnimationTitle();
}

void GameTitleSceneUI::Draw()
{
	//タイトルの描画
	titleSprite_->Draw();
	titleUISprite_->Draw();
}

void GameTitleSceneUI::AnimationTitle()
{
	//タイトルの文字を動かす
	titleSpriteMoveTimer_--;

	//開始座標
	float startY = titleSpritePosition_.y;

	//終了座標
	float endY = startY + titleSpriteMoveSpeed_;

	//移動
	const float lerpSpeed = 0.4f;
	float interpolatedY = Lerp(startY, endY, lerpSpeed);
	titleSpritePosition_.y = interpolatedY;

	//切り替えし
	if (titleSpriteMoveTimer_ < 0)
	{
		titleSpriteMoveSpeed_ *= kSpriteMoveSpeed_;
		titleSpriteMoveTimer_ = kMaxSpriteMoveTime_;
	}

	titleSprite_->SetPosition(titleSpritePosition_);
}