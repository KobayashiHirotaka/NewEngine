/**
 * @file ICharacterUI.h
 * @brief キャラクターのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/2D/Sprite/UI.h"

class ICharacterUI
{	
public:
	/// <summary>初期化</summary>
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;

	/// <summary>描画</summary>
	virtual void Draw() = 0;

	//UIの更新
	/// <summary>HPのUIの更新</summary>
	virtual void UpdateHPBar() = 0;

	/// <summary>ガードゲージのUIの更新</summary>
	virtual void UpdateGuardGaugeBar() = 0;

	/// <summary>ガードゲージのUIの適応</summary>
	virtual void AdjustGuardGauge() = 0;

	/// <summary>必殺技ゲージのUIの適応</summary>
	virtual void UpdateFinisherGaugeBar() = 0;

	/// <summary>必殺技ゲージのUIの適応</summary>
	virtual void AdjustFinisherGauge(float value) = 0;

	/// <summary>コンボ表示のUIの更新</summary>
	virtual void UpdateComboNumberSprite() = 0;
};

