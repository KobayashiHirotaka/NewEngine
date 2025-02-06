/**
 * @file PlayerUI.cpp
 * @brief プレイヤーのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "PlayerUI.h"
#include "Application/GameObject/Character/Player/Player.h"

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

void PlayerUI::Update()
{
	//体力の更新
	UpdateHPBar();
}

void PlayerUI::Draw()
{
	//体力ゲージの描画
	if (player_->GetBaseData().hp_ <= 0)
	{
		hpBar_.sprite_->Draw();
	}

	//ガードゲージの描画
	guardGaugeBar_.sprite_->Draw();

	//必殺技ゲージの描画
	finisherGaugeBar_.sprite_->Draw();

	//プレイヤーアイコンの描画
	playerIconSprite_->Draw();

	//コンボ表示の描画
	if (player_->GetComboCount() >= kComboCount_)
	{
		hitSprite_->Draw();
		comboNumSprite_->Draw();
	}
}

void PlayerUI::UpdateHPBar()
{
	//体力ゲージ
	const float kHpBarSizeY = 7.0f;
	const int kDivisionFactor = 2;
	const int kHalfHp = player_->GetBaseData().kMaxHp_ / kDivisionFactor;
	const int kQuarterHp = kHalfHp / kDivisionFactor;

	//色
	const Vector4 kDefaultHpColor = { 0.0f, 1.0f, 0.0f, 1.0f };
	const Vector4 kHalfHpColor = { 1.0f, 0.8f, 0.0f, 1.0f };
	const Vector4 kQuarterHpColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	//サイズを設定
	hpBar_.size_ = { (static_cast<float>(player_->GetBaseData().hp_) / static_cast<float>(player_->GetBaseData().kMaxHp_)) * barSize_, kHpBarSizeY };
	hpBar_.sprite_->SetSize(hpBar_.size_);

	//体力に応じて色を変化
	if (player_->GetBaseData().hp_ < -kHalfHp)
	{
		hpBar_.sprite_->SetColor(kDefaultHpColor);
	}
	else if (player_->GetBaseData().hp_ >= -kHalfHp && player_->GetBaseData().hp_ < -kQuarterHp)
	{
		hpBar_.sprite_->SetColor(kHalfHpColor);
	}
	else if (player_->GetBaseData().hp_ >= -kQuarterHp)
	{
		hpBar_.sprite_->SetColor(kQuarterHpColor);
	}
}
