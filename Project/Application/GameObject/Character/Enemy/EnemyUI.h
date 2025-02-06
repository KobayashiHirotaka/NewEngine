/**
 * @file EnemyUI.h
 * @brief 敵のUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Application/GameObject/Character/BaseCharacterUI.h"

class Enemy;

class EnemyUI : public BaseCharacterUI
{
public:
	/// <summary>デストラクタ</summary>
	virtual ~EnemyUI() = default;

	/// <summary>初期化</summary>
	void Initialize()override;

	/// <summary>更新</summary>
	void Update()override;

	/// <summary>描画</summary>
	void Draw()override;

	//UIの更新
	/// <summary>HPのUIの更新</summary>
	void UpdateHPBar()override;

	//Setter
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; };

private:
	//Sprite(hp)
	UI hpBar_;
	const float kBarSpace_ = 15.6f;
	float barSize_ = 480.0f;

	//Sprite(ガードゲージ)
	UI guardGaugeBar_;
	const float kGuardGaugeBarSpace_ = 48.5f;
	float guardGaugeBarSize_ = 240.0f;

	//Sprite(必殺技ゲージ)
	UI finisherGaugeBar_;
	const float kFinisherGaugeBarSpace_ = 627.0f;
	float finisherGaugeBarSize_ = 240.0f;

	//キャラクターアイコンのSprite
	std::unique_ptr<Sprite> enemyIconSprite_ = nullptr;
	uint32_t enemyIconTextureHandle_ = 0;

	//Sprite(コンボ表示)
	std::unique_ptr<Sprite> hitSprite_ = nullptr;
	uint32_t hitTextureHandle_;

	std::unique_ptr<Sprite> comboNumSprite_ = nullptr;
	uint32_t comboNumTextureHandle_;

	//Enemyのポインタ
	Enemy* enemy_;
};

