/**
 * @file GameSceneUI.h
 * @brief 各シーンのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2024/12/31
 */

#pragma once
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"

//操作説明の種類
enum class CommandSpriteType
{
	//基本操作
	GeneralCommandSprite,

	//コンボ攻撃
	ComboAttackCommandSprite,

	//必殺技攻撃
	FinisherAttackCommandSprite,
};

class GameSceneUI
{
public:
	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>更新</summary>
	void Update();

	/// <summary>描画</summary>
	void Draw();

	/// <summary>タイトルの文字を動かす</summary>
	void AnimationTitle();

	/// <summary>操作説明の更新</summary>
	void UpdateCommandSprite();

	/// <summary>操作説明の変更</summary>
	void ChangeCommandSprite();

	/// <summary>操作説明の適用</summary>
	void ApplyCommandSprite(int changeAmount);

	//Getter
	bool GetIsOpen() { return isOpen_; };

	bool GetIsChangedSprite() { return isChangedSprite_; };

	//Setter
	void SetIsChangedSprite(bool isChangedSprite) { isChangedSprite_ = isChangedSprite; };

private:
	//Input
	Input* input_ = nullptr;

	//Audio
	Audio* audio_ = nullptr;

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

	//基本操作説明用のSprite
	std::unique_ptr<Sprite> generalCommandListSprite_ = nullptr;
	uint32_t generalCommandListTextureHandle_ = 0;

	//攻撃操作説明用のSprite
	std::unique_ptr<Sprite> attackCommandListSprite_[2];
	uint32_t attackCommandListTextureHandle_[2];

	//何枚目のSpriteが表示されているか
	CommandSpriteType spriteCount_ = CommandSpriteType::GeneralCommandSprite;

	//スプライトの遷移用のベクター
	static const std::vector<CommandSpriteType> sSpriteOrder_;

	//操作説明が開かれているか
	bool isOpen_ = false;

	//操作説明が切り替わったか
	bool isChangedSprite_ = false;

	//スティック操作対応
	const float kStickDeadZone_ = 0.7f;
	const int kStickInputCooldownTime_ = 10;
	int stickInputCooldown_ = kStickInputCooldownTime_;
};

