/**
 * @file PlayerUI.cpp
 * @brief プレイヤーのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "PlayerUI.h"

void PlayerUI::Initialize()
{
	//リソース
	//体力ゲージ
	const Vector2 kHpBarPosition = { 537.0f, kBarSpace_ };
	const Vector2 kHpBarSize = { -barSize_  ,7.2f };

	hpBar_ = {
		true,
		Engine::TextureManager::LoadTexture("Resource/Images/HP.png"),
		kHpBarPosition,
		0.0f,
		kHpBarSize,
		nullptr,
	};

	hpBar_.sprite_.reset(Sprite::Create(hpBar_.textureHandle_, hpBar_.position_));

	//ガードゲージ
	const Vector2 kGuardGaugeBarPosition = { 537.0f, kGuardGaugeBarSpace_ };
	const Vector2 kGuardGaugeBarSize = { -guardGaugeBarSize_  ,7.0f };

	guardGaugeBar_ = {
		true,
		Engine::TextureManager::LoadTexture("Resource/Images/GuardGauge.png"),
		{kGuardGaugeBarPosition},
		0.0f,
		{kGuardGaugeBarSize},
		nullptr,
	};

	guardGaugeBar_.sprite_.reset(Sprite::Create(guardGaugeBar_.textureHandle_, guardGaugeBar_.position_));

	//必殺技ゲージ
	const Vector2 kFinisherGaugeBarPosition = { 299.0f, kFinisherGaugeBarSpace_ };
	const Vector2 kFinisherGaugeBarSize = { -finisherGaugeBarSize_  ,19.3f };

	finisherGaugeBar_ = {
		true,
		Engine::TextureManager::LoadTexture("Resource/Images/FinisherGauge.png"),
		{kFinisherGaugeBarPosition},
		0.0f,
		{kFinisherGaugeBarSize},
		nullptr,
	};

	finisherGaugeBar_.sprite_.reset(Sprite::Create(finisherGaugeBar_.textureHandle_, finisherGaugeBar_.position_));

	//ヒット表示
	const Vector2 kHitSpritePosition = { 1090.0f, 180.0f };

	hitTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/Hit.png");
	hitSprite_.reset(Sprite::Create(hitTextureHandle_, kHitSpritePosition));

	//コンボ表示
	const Vector2 kComboNumSpritePosition = { 1060.0f, 290.0f };

	comboNumTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Number/0.png");
	comboNumSprite_.reset(Sprite::Create(comboNumTextureHandle_, kComboNumSpritePosition));

	//キャラクターアイコン
	const Vector2 kPlayerIconPosition = { 53.0f, 20.0f };
	const Vector2 kPlayerIconSize = { 120.0f,120.0f };

	playerIconTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/PlayerIcon.png");

	playerIconSprite_.reset(Sprite::Create(playerIconTextureHandle_, kPlayerIconPosition));
	playerIconSprite_->SetSize(kPlayerIconSize);
}