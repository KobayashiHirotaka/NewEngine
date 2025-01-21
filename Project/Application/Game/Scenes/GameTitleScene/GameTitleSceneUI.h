/**
 * @file GameTitleSceneUI.h
 * @brief タイトルシーンのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2024/01/15
 */

#pragma once
#include "Application/Game/Scenes/IGameSceneUI.h"

class GameTitleSceneUI : public IGameSceneUI
{
public:
	/// <summary>初期化</summary>
	void Initialize()override;

	/// <summary>更新</summary>
	void Update()override;

	/// <summary>描画</summary>
	void Draw()override;

private:
	/// <summary>タイトルの文字を動かす</summary>
	void AnimationTitle();

private:
	//タイトル(モノクロファイター)のSprite
	std::unique_ptr<Sprite> titleSprite_ = nullptr;
	uint32_t titleTextureHandle_ = 0;
	Vector2 titleSpritePosition_ = { 0.0f,-25.0f };
	float titleSpriteMoveSpeed_ = 1.5f;
	const float kSpriteMoveSpeed_ = -1.0f;
	const int kMaxSpriteMoveTime_ = 30;
	int titleSpriteMoveTimer_ = kMaxSpriteMoveTime_;

	//タイトル(操作用)のSprite
	std::unique_ptr<Sprite> titleUISprite_ = nullptr;
	uint32_t titleUITextureHandle_ = 0;
};

