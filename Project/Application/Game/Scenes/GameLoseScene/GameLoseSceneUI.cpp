/**
 * @file GameWinSceneUI.cpp
 * @brief 勝利シーンのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2024/01/16
 */

#include "GameLoseSceneUI.h"

void GameLoseSceneUI::Initialize()
{
	//UIの初期化
	//Lose表示のSprite
	loseSceneTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/LoseScene.png");
	loseSceneSprite_.reset(Sprite::Create(loseSceneTextureHandle_, { 0.0f,0.0f }));
}

void GameLoseSceneUI::Update()
{

}

void GameLoseSceneUI::Draw()
{
	//Loseの描画
	loseSceneSprite_->Draw();
}