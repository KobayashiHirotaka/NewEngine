/**
 * @file EnemyUI.cpp
 * @brief 敵のUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "EnemyUI.h"
#include "Application/GameObject/Character/Enemy/Enemy.h"

void EnemyUI::Initialize()
{
	//リソース
	//体力ゲージ
	const Vector2 kHpBarPosition = { 742.0f, kBarSpace_ };
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
	const Vector2 kGuardGaugeBarPosition = { 742.0f, kGuardGaugeBarSpace_ };
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
	const Vector2 kFinisherGaugeBarPosition = { 979.0f, kFinisherGaugeBarSpace_ };
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
	const Vector2 kHitSpritePosition = { 40.0f, 180.0f };

	hitTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/Hit.png");
	hitSprite_.reset(Sprite::Create(hitTextureHandle_, kHitSpritePosition));

	//コンボ表示
	const Vector2 kComboNumSpritePosition = { 10.0f, 290.0f };

	comboNumTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Number/0.png");
	comboNumSprite_.reset(Sprite::Create(comboNumTextureHandle_, kComboNumSpritePosition));

	//キャラクターアイコン
	const Vector2 kEnemyIconPosition = { 1110.0f, 20.0f };
	const Vector2 kEnemyIconSize = { 120.0f,120.0f };

	enemyIconTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/EnemyIcon.png");

	enemyIconSprite_.reset(Sprite::Create(enemyIconTextureHandle_, kEnemyIconPosition));
	enemyIconSprite_->SetSize(kEnemyIconSize);
}

void EnemyUI::Update()
{
	//体力の更新
	UpdateHPBar();
}

void EnemyUI::Draw()
{
	//体力ゲージの描画
	if (enemy_->GetBaseData().hp_ >= 0)
	{
		hpBar_.sprite_->Draw();
	}

	//ガードゲージの描画
	guardGaugeBar_.sprite_->Draw();

	//必殺技ゲージの描画
	finisherGaugeBar_.sprite_->Draw();

	//エネミーアイコンの描画
	enemyIconSprite_->Draw();

	//コンボ表示の描画
	if (enemy_->GetComboCount() >= kComboCount_)
	{
		hitSprite_->Draw();
		comboNumSprite_->Draw();
	}
}

void EnemyUI::UpdateHPBar()
{
	//体力ゲージ
	const float kHpBarSizeY = 7.0f;
	const int kDivisionFactor = 2;
	const int kHalfHp = enemy_->GetBaseData().kMaxHp_ / kDivisionFactor;
	const int kQuarterHp = kHalfHp / kDivisionFactor;

	//色
	const Vector4 kDefaultHpColor = { 0.0f, 1.0f, 0.0f, 1.0f };
	const Vector4 kHalfHpColor = { 1.0f, 0.8f, 0.0f, 1.0f };
	const Vector4 kQuarterHpColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	//サイズを設定
	hpBar_.size_ = { (static_cast<float>(enemy_->GetBaseData().hp_) / static_cast<float>(enemy_->GetBaseData().kMaxHp_)) * barSize_, kHpBarSizeY };
	hpBar_.sprite_->SetSize(hpBar_.size_);

	//体力に応じて色を変化
	if (enemy_->GetBaseData().hp_ > kHalfHp)
	{
		hpBar_.sprite_->SetColor(kDefaultHpColor);
	}

	if (enemy_->GetBaseData().hp_ <= kHalfHp && enemy_->GetBaseData().hp_ > kQuarterHp)
	{
		hpBar_.sprite_->SetColor(kHalfHpColor);
	}
	else if (enemy_->GetBaseData().hp_ <= kQuarterHp)
	{
		hpBar_.sprite_->SetColor(kQuarterHpColor);
	}
}

