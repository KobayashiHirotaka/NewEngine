/**
 * @file GameTitleScene.h
 * @brief タイトルシーンの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "IScene.h"
#include "Application/GameObject/Skydome/Skydome.h"

class GameTitleScene : public IScene
{
public:
	/// <summary>コンストラクタ</summary>
	GameTitleScene();

	/// <summary>デストラクタ</summary>
	~GameTitleScene();

	/// <summary>初期化</summary>
	void Initialize()override;

	/// <summary>更新</summary>
	void Update()override;

	/// <summary>描画</summary>
	void Draw()override;

	/// <summary>終了</summary>
	void Finalize()override;

	/// <summary>ImGui</summary>
	void ImGui()override;

private:
	/// <summary>タイトルの文字を動かす</summary>
	void AnimationTitle();

	/// <summary>操作説明の更新</summary>
	void UpdateCommandSprite();

	/// <summary>操作説明の変更</summary>
	void ChangeCommandSprite();

	/// <summary>操作説明の適用</summary>
	void ApplyCommandSprite(int changeAmount);

private:
	//TextureManager
	TextureManager* textureManager_ = nullptr;

	//ModelManager
	ModelManager* modelManager_ = nullptr;

	//Input
	Input* input_ = nullptr;

	//Audio
	Audio* audio_ = nullptr;

	//Camera
	Camera camera_;
	DebugCamera debugCamera_;
	bool isDebugCamera_ = false;

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

	//サウンド
	uint32_t titleSoundHandle_ = 0u;
	uint32_t selectSoundHandle_ = 0u;

	//大きさ
	float volume_ = 1.0f;

	//再生されているか
	bool isPlayAudio_ = false;

	//何枚目のSpriteが表示されているか
	CommandSpriteType spriteCount_ = CommandSpriteType::GeneralCommandSprite;

	//操作説明が開かれているか
	bool isOpen_ = false;

	//Skydome
	std::unique_ptr<Skydome> skydome_;

	//スティック操作対応
	const float kValue_ = 0.7f;
	const int kStickInputCooldownTime_ = 10;
	int stickInputCooldown_ = kStickInputCooldownTime_;
};
