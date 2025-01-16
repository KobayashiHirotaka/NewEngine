/**
 * @file GameWinSceneUI.cpp
 * @brief 勝利シーンのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2024/01/16
 */

#include "GameWinSceneUI.h"

void GameWinSceneUI::Initialize()
{
	//UIの初期化
	//Win表示のSprite
	winSceneTextureHandle_ = TextureManager::LoadTexture("Resource/Images/WinScene.png");
	winSceneSprite_.reset(Sprite::Create(winSceneTextureHandle_, { 0.0f,0.0f }));
}

void GameWinSceneUI::Update()
{

}

void GameWinSceneUI::Draw()
{
	//Winの描画
	winSceneSprite_->Draw();
}